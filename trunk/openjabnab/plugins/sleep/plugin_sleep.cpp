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

PluginSleep::PluginSleep():PluginInterface("sleep", "Advanced sleep and wake up") {}

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

	bunny->SendPacket(SleepPacket(SleepPacket::Wake_Up));
}

void PluginSleep::CleanCrons(Bunny * b)
{
	Cron::UnregisterAllForBunny(this, b);
}

void PluginSleep::RegisterCrons(Bunny * b)
{
	QStringList wakeupList = b->GetPluginSetting(GetName(), QString("wakeupList"), QStringList()).toStringList();
	QStringList sleepList = b->GetPluginSetting(GetName(), QString("sleepList"), QStringList()).toStringList();

	if(wakeupList.count() == 0 && sleepList.count() == 0) // Nothing configured
		return;

	if(wakeupList.count() != 7 || sleepList.count() != 7)
	{
		LogError("Bad list size");
		return;
	}

	for(int day = 0; day < 7; day++)
	{
		Cron::RegisterWeekly(this, (Qt::DayOfWeek)(day+1), QTime::fromString(wakeupList.at(day), "hh:mm"), b, QVariant(), "OnCronWakeUp");
		Cron::RegisterWeekly(this, (Qt::DayOfWeek)(day+1), QTime::fromString(sleepList.at(day), "hh:mm"), b, QVariant(), "OnCronSleep");
	}
}

/*******
 * API *
 *******/
 
void PluginSleep::InitApiCalls()
{
	DECLARE_PLUGIN_BUNNY_API_CALL("goodNight", PluginSleep, Api_GoodNight);
	DECLARE_PLUGIN_BUNNY_API_CALL("hello", PluginSleep, Api_Hello);
	DECLARE_PLUGIN_BUNNY_API_CALL("setup", PluginSleep, Api_Setup);
}

PLUGIN_BUNNY_API_CALL(PluginSleep::Api_GoodNight)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);
	
	if(!bunny->IsIdle())
		return new ApiManager::ApiError(QString("Bunny is not idle"));

	bunny->SendPacket(SleepPacket(SleepPacket::Sleep));
	return new ApiManager::ApiOk(QString("Bunny is going to sleep."));
}

PLUGIN_BUNNY_API_CALL(PluginSleep::Api_Hello)
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

	if(!hRequest.HasArg("wakeupList"))
		return new ApiManager::ApiError(QString("Missing argument 'wakeupList'"));

	if(!hRequest.HasArg("sleepList"))
		return new ApiManager::ApiError(QString("Missing argument 'sleepList'"));
		
	QStringList wakeupList =  hRequest.GetArg("wakeupList").split(',');
	QStringList sleepList = hRequest.GetArg("sleepList").split(',');
	
	if(wakeupList.count() != 7 || sleepList.count() != 7)
		return new ApiManager::ApiError(QString("Bad list size"));
	
	bunny->SetPluginSetting(GetName(), "wakeupList", wakeupList);
	bunny->SetPluginSetting(GetName(), "sleepList", sleepList);
	
	CleanCrons(bunny);
	RegisterCrons(bunny);
	
	return new ApiManager::ApiOk(QString("Plugin configuration updated."));
}
