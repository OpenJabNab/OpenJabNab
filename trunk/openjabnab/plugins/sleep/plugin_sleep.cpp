// typedef QPair<int, QTime> Week[7];
// QMap<Bunny *, Week>
// Week const& weekList = map.value( b );
// for (i = 0; i < 7; i++)
// 	Cron::Unregister(this, week[i].first);

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
	for (int i = 0; i < 7; i++)
	{
		Cron::Unregister(this, sleepList->listOfCrons[i]);
		Cron::Unregister(this, wakeupList->listOfCrons[i]);
	}
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
	Week* const sleepList = listOfSleep.value( b );
	Week* const wakeupList = listOfWakeUp.value( b );
	for (int i = 0; i < 7; i++)
	for(int day = 0; day < 7; day++)
	{
		QString WakeUp = b->GetPluginSetting(GetName(), QString("%1/WakeUp").arg(QDate::longDayName(day+1)), QString("08:00")).toString();
		wakeupList->listOfCrons[day] = Cron::RegisterWeekly(this, (Qt::DayOfWeek)(day+1), QTime::fromString(WakeUp, "hh:mm"), QVariant::fromValue( b ), "OnCronWakeUp");
		wakeupList->listOfTimes[day] = QTime::fromString(WakeUp, "hh:mm");
		QString Sleep = b->GetPluginSetting(GetName(), QString("%1/Sleep").arg(QDate::longDayName(day+1)), QString("22:00")).toString();
		sleepList->listOfCrons[day] = Cron::RegisterWeekly(this, (Qt::DayOfWeek)(day+1), QTime::fromString(Sleep, "hh:mm"), QVariant::fromValue( b ), "OnCronSleep");
		sleepList->listOfTimes[day] = QTime::fromString(Sleep, "hh:mm");
	}
	listOfWakeUp.insert(b, wakeupList);
	listOfSleep.insert(b, sleepList);
}

ApiManager::ApiAnswer * PluginSleep::ProcessBunnyApiCall(Bunny * b, Account const&, QString const& funcName, HTTPRequest const& r)
{
	if(funcName == "goodNight")
	{
		b->SendPacket(SleepPacket(SleepPacket::Sleep));
		return new ApiManager::ApiOk(QString("Bunny is going to sleep."));
	}
	else if(funcName == "hello")
	{
		b->SendPacket(SleepPacket(SleepPacket::Wake_Up));
		return new ApiManager::ApiOk(QString("Bunny is waking up."));
	}
	else if(funcName == "sleep")
	{
		if(!r.HasArg("time"))
			return new ApiManager::ApiError(QString("Missing argument 'time' for plugin " + GetVisualName()));

		if(!r.HasArg("day"))
			return new ApiManager::ApiError(QString("Missing argument 'day' for plugin " + GetVisualName()));

		b->SetPluginSetting(GetName(), QString("%1/Sleep").arg(r.GetArg("day")), QString(r.GetArg("time")));
		return new ApiManager::ApiOk(QString("Plugin configuration updated."));
	}
	else if(funcName == "wakup")
	{
		if(!r.HasArg("time"))
			return new ApiManager::ApiError(QString("Missing argument 'time' for plugin " + GetVisualName()));

		if(!r.HasArg("day"))
			return new ApiManager::ApiError(QString("Missing argument 'day' for plugin " + GetVisualName()));

		b->SetPluginSetting(GetName(), QString("%1/WakeUp").arg(r.GetArg("day")), QString(r.GetArg("time")));
		return new ApiManager::ApiOk(QString("Plugin configuration updated."));
	}
	else
		return new ApiManager::ApiError(QString("Bad function name for plugin " + GetVisualName()));
}
