#include <QDate>
#include <QMap>
#include "plugin_sleep.h"
#include "bunny.h"
#include "cron.h"
#include "log.h"
#include "messagepacket.h"
#include "packet.h"
#include "settings.h"
#include "sleeppacket.h"

Q_EXPORT_PLUGIN2(plugin_sleep, PluginSleep)

PluginSleep::PluginSleep():PluginInterface("sleep", "Advanced sleep and wake up",BunnyPlugin) {}

PluginSleep::~PluginSleep()
{
	Cron::UnregisterAll(this);
}

void PluginSleep::OnBunnyConnect(Bunny * b)
{
	RegisterCrons(b);
}

void PluginSleep::OnBunnyDisconnect(Bunny * b)
{
	CleanCrons(b);
}

bool PluginSleep::IsConfigValid(QList<QVariant> const& wakeupList, QList<QVariant> const& sleepList)
{
	if(wakeupList.count() == 0 && sleepList.count() == 0) // Nothing configured, nothing to do
		return false;

	if(wakeupList.count() != 7 || sleepList.count() != 7) // Error :/
	{
		LogError("Bad list size");
		return false;
	}
	return true;
}

void PluginSleep::OnInitPacket(const Bunny * b, AmbientPacket &, SleepPacket & s)
{
	// Check if bunny need to sleep or not
	QList<QVariant> wakeupList = b->GetPluginSetting(GetName(), QString("wakeupList"), QList<QVariant>()).toList();
	QList<QVariant> sleepList = b->GetPluginSetting(GetName(), QString("sleepList"), QList<QVariant>()).toList();

	if(!IsConfigValid(wakeupList, sleepList))
		return;

	QTime currentTime = QTime::currentTime();
	int day = QDate::currentDate().dayOfWeek()-1;

	if (wakeupList.at(day).toTime() <= currentTime && currentTime < sleepList.at(day).toTime())
		s.SetState(SleepPacket::Wake_Up);
	else
		s.SetState(SleepPacket::Sleep);
}

void PluginSleep::UpdateState(Bunny * b)
{
	// Check if bunny need to sleep or not
	QList<QVariant> wakeupList = b->GetPluginSetting(GetName(), QString("wakeupList"), QList<QVariant>()).toList();
	QList<QVariant> sleepList = b->GetPluginSetting(GetName(), QString("sleepList"), QList<QVariant>()).toList();

	if(!IsConfigValid(wakeupList, sleepList))
		return;

	QTime currentTime = QTime::currentTime();
	int day = QDate::currentDate().dayOfWeek()-1;

	bool wakeup = (wakeupList.at(day).toTime() <= currentTime && currentTime < sleepList.at(day).toTime());

	if (wakeup && b->IsSleeping())
		b->SendPacket(SleepPacket(SleepPacket::Wake_Up));
	else if(!wakeup && b->IsIdle())
		b->SendPacket(SleepPacket(SleepPacket::Sleep));
}

void PluginSleep::OnCronWakeUp(Bunny * bunny, QVariant)
{
	if(!bunny->IsSleeping())
	{
		LogWarning("WakeUp but not sleeping.");
		return;
	}
	bunny->SendPacket(SleepPacket(SleepPacket::Wake_Up));
}


void PluginSleep::OnCronSleep(Bunny * bunny, QVariant)
{
	if(!bunny->IsIdle())
	{
		LogWarning("Sleep but not idle.");
		return;
	}
	bunny->SendPacket(SleepPacket(SleepPacket::Sleep));
}

void PluginSleep::CleanCrons(Bunny * b)
{
	Cron::UnregisterAllForBunny(this, b);
}

void PluginSleep::RegisterCrons(Bunny * b)
{
	QList<QVariant> wakeupList = b->GetPluginSetting(GetName(), QString("wakeupList"), QList<QVariant>()).toList();
	QList<QVariant> sleepList = b->GetPluginSetting(GetName(), QString("sleepList"), QList<QVariant>()).toList();

	if(wakeupList.count() == 0 && sleepList.count() == 0) // Nothing configured
		return;

	if(wakeupList.count() != 7 || sleepList.count() != 7)
	{
		LogError("Bad list size");
		return;
	}

	for(int day = 0; day < 7; day++)
	{
		Cron::RegisterWeekly(this, (Qt::DayOfWeek)(day+1), wakeupList.at(day).toTime(), b, QVariant(), "OnCronWakeUp");
		Cron::RegisterWeekly(this, (Qt::DayOfWeek)(day+1), sleepList.at(day).toTime(), b, QVariant(), "OnCronSleep");
	}
}

/*******
 * API *
 *******/

void PluginSleep::InitApiCalls()
{
	DECLARE_PLUGIN_BUNNY_API_CALL("sleep()", PluginSleep, Api_Sleep);
	DECLARE_PLUGIN_BUNNY_API_CALL("wakeup()", PluginSleep, Api_Wakeup);
	DECLARE_PLUGIN_BUNNY_API_CALL("setup(wakeupList,sleepList)", PluginSleep, Api_Setup);
	DECLARE_PLUGIN_BUNNY_API_CALL("getsetup()", PluginSleep, Api_GetSetup);
}

PLUGIN_BUNNY_API_CALL(PluginSleep::Api_Sleep)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);

	if(!bunny->IsIdle())
		return new ApiManager::ApiError(QString("Bunny is not idle"));

	bunny->SendPacket(SleepPacket(SleepPacket::Sleep));
	return new ApiManager::ApiOk(QString("Bunny is going to sleep."));
}

PLUGIN_BUNNY_API_CALL(PluginSleep::Api_Wakeup)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);

	if(!bunny->IsSleeping())
		return new ApiManager::ApiError(QString("Bunny is not sleeping"));

	bunny->SendPacket(SleepPacket(SleepPacket::Wake_Up));
	return new ApiManager::ApiOk(QString("Bunny is waking up."));
}

PLUGIN_BUNNY_API_CALL(PluginSleep::Api_Setup)
{
	Q_UNUSED(account);

	QStringList wakeupList =  hRequest.GetArg("wakeupList").split(',');
	QStringList sleepList = hRequest.GetArg("sleepList").split(',');

	if(wakeupList.count() != 7 || sleepList.count() != 7)
		return new ApiManager::ApiError(QString("Bad list size"));

	// Transform QStringList to QList<QTime>
	QList<QVariant> wList;
	QList<QVariant> sList;
	for(int day = 0; day < 7; ++day)
	{
		QTime w = QTime::fromString(wakeupList.at(day), "hh:mm");
		if(w.isValid())
			wList.append(w);
		else
			return new ApiManager::ApiError(QString("Bad time '%1'").arg(wakeupList.at(day)));

		QTime s = QTime::fromString(sleepList.at(day), "hh:mm");
		if(s.isValid())
			sList.append(s);
		else
			return new ApiManager::ApiError(QString("Bad time '%1'").arg(sleepList.at(day)));
	}

	bunny->SetPluginSetting(GetName(), "wakeupList", wList);
	bunny->SetPluginSetting(GetName(), "sleepList", sList);

	CleanCrons(bunny);
	RegisterCrons(bunny);
	UpdateState(bunny);

	return new ApiManager::ApiOk(QString("Plugin configuration updated."));
}

PLUGIN_BUNNY_API_CALL(PluginSleep::Api_GetSetup)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);

	QStringList setup;
	QList<QVariant> wakeupList = bunny->GetPluginSetting(GetName(), QString("wakeupList"), QList<QVariant>()).toList();
	QList<QVariant> sleepList = bunny->GetPluginSetting(GetName(), QString("sleepList"), QList<QVariant>()).toList();
	if(wakeupList.count() == 7 && sleepList.count() == 7)
	{
		for(int day = 0; day < 7; day++)
		{
			setup << wakeupList.at(day).toString();
		}
		for(int day = 0; day < 7; day++)
		{
			setup << sleepList.at(day).toString();
		}
	}

	return new ApiManager::ApiList(setup);
}
