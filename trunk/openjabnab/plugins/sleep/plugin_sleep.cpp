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

PluginSleep::PluginSleep():PluginInterface("sleep", "Advanced sleep and wake up")
{
}

PluginSleep::~PluginSleep() {}

void PluginSleep::OnBunnyConnect(Bunny * b)
{
	RegisterCrons(b);
}

void PluginSleep::OnBunnyDisconnect(Bunny * b)
{
	Week* const sleepList = listOfSleep.value( b );
	Week* const wakeupList = listOfWakeUp.value( b );
	if(sleepList && wakeupList)
	{
		for (int i = 0; i < 7; i++)
		{
			if(sleepList->listOfCrons[i])
				Cron::Unregister(this, sleepList->listOfCrons[i]);
			if(wakeupList->listOfCrons[i])
				Cron::Unregister(this, wakeupList->listOfCrons[i]);
		}
	}
	delete sleepList;
	delete wakeupList;
}

void PluginSleep::OnCronSleep()
{
}

void PluginSleep::OnCronWakeUp()
{
}

void PluginSleep::RegisterCrons(Bunny * b)
{
	OnBunnyDisconnect(b);
	Week* const sleepList = new Week;
	Week* const wakeupList = new Week;
	QStringList LWakeUp = b->GetPluginSetting(GetName(), QString("Horaires/WakeUp"), QStringList()).toStringList();	/* On fait les listes avant la boucle */
	QStringList LSleep = b->GetPluginSetting(GetName(), QString("Horaires/Sleep"), QStringList()).toStringList();
	QString WakeUp,Sleep;
	for(int day = 0; day < 7; day++)
	{
		if(LWakeUp.size() > day) { /* Si y'a plus d'items dans la liste que c'qu'on veut accéder, on peut l'faire */
			WakeUp = LWakeUp[day];
			if(WakeUp != NULL && WakeUp.length() == 5)
			{
				wakeupList->listOfCrons[day] = Cron::RegisterWeekly(this, (Qt::DayOfWeek)(day+1), QTime::fromString(WakeUp, "hh:mm"), QVariant::fromValue( b ), "OnCronWakeUp");
				wakeupList->listOfTimes[day] = QTime::fromString(WakeUp, "hh:mm");
			}
		}
		if(LSleep.size() > day) {
			Sleep = LSleep[day];
			if(Sleep != NULL && Sleep.length() == 5)
			{
				sleepList->listOfCrons[day] = Cron::RegisterWeekly(this, (Qt::DayOfWeek)(day+1), QTime::fromString(Sleep, "hh:mm"), QVariant::fromValue( b ), "OnCronSleep");
				sleepList->listOfTimes[day] = QTime::fromString(Sleep, "hh:mm");
			}
		}
	}
	listOfWakeUp.insert(b, wakeupList);
	listOfSleep.insert(b, sleepList);
}

void PluginSleep::InitApiCalls()
{
	DECLARE_PLUGIN_BUNNY_API_CALL("goodNight", PluginSleep, Api_GoodNight);
	DECLARE_PLUGIN_BUNNY_API_CALL("hello", PluginSleep, Api_Hello);
	DECLARE_PLUGIN_BUNNY_API_CALL("sleep", PluginSleep, Api_Sleep);
	DECLARE_PLUGIN_BUNNY_API_CALL("wakeup", PluginSleep, Api_WakeUp);
}

PLUGIN_BUNNY_API_CALL(PluginSleep::Api_GoodNight)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);

	bunny->SendPacket(SleepPacket(SleepPacket::Sleep));
	return new ApiManager::ApiOk(QString("Bunny is going to sleep."));
}

PLUGIN_BUNNY_API_CALL(PluginSleep::Api_Hello)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);

	bunny->SendPacket(SleepPacket(SleepPacket::Wake_Up));
	return new ApiManager::ApiOk(QString("Bunny is waking up."));
}

PLUGIN_BUNNY_API_CALL(PluginSleep::Api_Sleep)
{
	Q_UNUSED(account);

	if(!hRequest.HasArg("time"))
		return new ApiManager::ApiError(QString("Missing argument 'time' for plugin " + GetVisualName()));

	if(!hRequest.HasArg("day"))
		return new ApiManager::ApiError(QString("Missing argument 'day' for plugin " + GetVisualName()));

	bunny->SetPluginSetting(GetName(), QString("%1/Sleep").arg(hRequest.GetArg("day")), QString(hRequest.GetArg("time")));
	return new ApiManager::ApiOk(QString("Plugin configuration updated."));
}

PLUGIN_BUNNY_API_CALL(PluginSleep::Api_WakeUp)
{
	Q_UNUSED(account);

	if(!hRequest.HasArg("time"))
		return new ApiManager::ApiError(QString("Missing argument 'time' for plugin " + GetVisualName()));

	if(!hRequest.HasArg("day"))
		return new ApiManager::ApiError(QString("Missing argument 'day' for plugin " + GetVisualName()));

	bunny->SetPluginSetting(GetName(), QString("%1/WakeUp").arg(hRequest.GetArg("day")), QString(hRequest.GetArg("time")));
	return new ApiManager::ApiOk(QString("Plugin configuration updated."));
}
