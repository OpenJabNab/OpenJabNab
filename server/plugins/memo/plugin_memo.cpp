#include <QDateTime>
#include <QCryptographicHash>
#include <QMapIterator>
#include <QRegExp>
#include <memory>
#include "bunny.h"
#include "bunnymanager.h"
#include "httprequest.h"
#include "log.h"
#include "cron.h"
#include "messagepacket.h"
#include "plugin_memo.h"
#include "settings.h"
#include "ttsmanager.h"

Q_EXPORT_PLUGIN2(plugin_memo, PluginMemo)

PluginMemo::PluginMemo():PluginInterface("memo", "Memo", BunnyPlugin)
{
	std::auto_ptr<QDir> dir(GetLocalHTTPFolder());
	if(dir.get())
	{
		memoFolder = *dir;
	}
}

PluginMemo::~PluginMemo()
{
	Cron::UnregisterAll(this);
}

void PluginMemo::OnCron(Bunny * b, QVariant v)
{
	QByteArray msg = TTSManager::CreateNewSound(v.value<QString>(), "fr", "google");
	QByteArray message = "MU "+msg+"\nPL 3\nMW\n";
	b->SendPacket(MessagePacket(message));
}

void PluginMemo::OnBunnyConnect(Bunny * b)
{
	QMap<QString, QVariant> listDaily = b->GetPluginSetting(GetName(), "DailyWebcasts", QMap<QString, QVariant>()).toMap();
	QMapIterator<QString, QVariant> i(listDaily);
	while (i.hasNext()) {
		i.next();
		QString time = i.key();
		QString message = i.value().toString();
		Cron::RegisterDaily(this, QTime::fromString(time, "hh:mm"), b, QVariant::fromValue(message));
	}
	QMap<QString, QVariant> list = b->GetPluginSetting(GetName(), "Webcasts", QMap<QString, QVariant>()).toMap();
	QMapIterator<QString, QVariant> i2(list);
	while (i2.hasNext()) {
		i2.next();
		QStringList when = i2.key().split("|");
		int day = when.at(0).toInt();
		QString time = when.at(1);
		QString message = i2.value().toString();
		Cron::RegisterWeekly(this, (Qt::DayOfWeek)day, QTime::fromString(time, "hh:mm"), b, QVariant::fromValue(message));
	}
}

void PluginMemo::OnBunnyDisconnect(Bunny * b)
{
	Cron::UnregisterAllForBunny(this, b);
}

void PluginMemo::InitApiCalls()
{
	DECLARE_PLUGIN_BUNNY_API_CALL("addwebcast(day,time,message)", PluginMemo, Api_AddWebcast);
	DECLARE_PLUGIN_BUNNY_API_CALL("removewebcast(day,time)", PluginMemo, Api_RemoveWebcast);
	DECLARE_PLUGIN_BUNNY_API_CALL("getwebcastslist()", PluginMemo, Api_ListWebcast);
	DECLARE_PLUGIN_BUNNY_API_CALL("adddaily(time,message)", PluginMemo, Api_AddDailyWebcast);
	DECLARE_PLUGIN_BUNNY_API_CALL("removedaily(time)", PluginMemo, Api_RemoveDailyWebcast);
	DECLARE_PLUGIN_BUNNY_API_CALL("getdailylist()", PluginMemo, Api_ListDailyWebcast);
}

PLUGIN_BUNNY_API_CALL(PluginMemo::Api_AddWebcast)
{
	Q_UNUSED(account);

	QString hTime = hRequest.GetArg("time");
	int hDay = hRequest.GetArg("day").toInt();
	QString message = hRequest.GetArg("message");
	QMap<QString, QVariant> list = bunny->GetPluginSetting(GetName(), "Webcasts", QMap<QString, QVariant>()).toMap();
	if(!list.contains(QString::number(hDay)+"|"+hTime))
	{
		Cron::RegisterWeekly(this, (Qt::DayOfWeek)hDay, QTime::fromString(hTime, "hh:mm"), bunny, QVariant::fromValue(message));
		list.insert(QString::number(hDay)+"|"+hTime,message);
		bunny->SetPluginSetting(GetName(), "Webcasts", list);
		return new ApiManager::ApiOk(QString("Add webcast at '%1' to bunny '%2'").arg(hTime, QString(bunny->GetID())));
	}
	return new ApiManager::ApiError(QString("Webcast already exists at '%1' for bunny '%2'").arg(hTime, QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginMemo::Api_RemoveWebcast)
{
	Q_UNUSED(account);

	int hDay = hRequest.GetArg("day").toInt();
	if(!hRequest.HasArg("time"))
		return new ApiManager::ApiError(QString("Missing argument 'time' for plugin Memo"));

	QMap<QString, QVariant> list = bunny->GetPluginSetting(GetName(), "Webcasts", QMap<QString, QVariant>()).toMap();
	QString time = hRequest.GetArg("time");
    if(list.contains(QString::number(hDay)+"|"+time))
    {
		list.remove(QString::number(hDay)+"|"+time);
        bunny->SetPluginSetting(GetName(), "Webcasts", list);

		// Recreate crons
        OnBunnyDisconnect(bunny);
        OnBunnyConnect(bunny);
        return new ApiManager::ApiOk(QString("Remove webcast at '%1' for bunny '%2'").arg(hRequest.GetArg("time"), QString(bunny->GetID())));
    }
    return new ApiManager::ApiError(QString("No webcast at '%1' for bunny '%2'").arg(hRequest.GetArg("time"), QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginMemo::Api_ListWebcast)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);
	return new ApiManager::ApiMappedList(bunny->GetPluginSetting(GetName(), "Webcasts", QMap<QString, QVariant>()).toMap());
}

PLUGIN_BUNNY_API_CALL(PluginMemo::Api_AddDailyWebcast)
{
	Q_UNUSED(account);

	QString hTime = hRequest.GetArg("time");
	QString message = hRequest.GetArg("message");
	QMap<QString, QVariant> list = bunny->GetPluginSetting(GetName(), "DailyWebcasts", QMap<QString, QVariant>()).toMap();
	if(!list.contains(hTime))
	{
		Cron::RegisterDaily(this, QTime::fromString(hTime, "hh:mm"), bunny, QVariant::fromValue(message));
		list.insert(hTime,message);
		bunny->SetPluginSetting(GetName(), "DailyWebcasts", list);
		return new ApiManager::ApiOk(QString("Add webcast at '%1' to bunny '%2'").arg(hTime, QString(bunny->GetID())));
	}
	return new ApiManager::ApiError(QString("Webcast already exists at '%1' for bunny '%2'").arg(hTime, QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginMemo::Api_RemoveDailyWebcast)
{
	Q_UNUSED(account);

	if(!hRequest.HasArg("time"))
		return new ApiManager::ApiError(QString("Missing argument 'time' for plugin Memo"));

	QMap<QString, QVariant> list = bunny->GetPluginSetting(GetName(), "DailyWebcasts", QMap<QString, QVariant>()).toMap();
	QString time = hRequest.GetArg("time");
    if(list.contains(time))
    {
		list.remove(time);
        bunny->SetPluginSetting(GetName(), "DailyWebcasts", list);

		// Recreate crons
        OnBunnyDisconnect(bunny);
        OnBunnyConnect(bunny);
        return new ApiManager::ApiOk(QString("Remove webcast at '%1' for bunny '%2'").arg(hRequest.GetArg("time"), QString(bunny->GetID())));
    }
    return new ApiManager::ApiError(QString("No webcast at '%1' for bunny '%2'").arg(hRequest.GetArg("time"), QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginMemo::Api_ListDailyWebcast)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);
	return new ApiManager::ApiMappedList(bunny->GetPluginSetting(GetName(), "DailyWebcasts", QMap<QString, QVariant>()).toMap());
}
