#include <QDateTime>
#include <QMapIterator>
#include <memory>
#include "bunny.h"
#include "bunnymanager.h"
#include "httprequest.h"
#include "log.h"
#include "cron.h"
#include "messagepacket.h"
#include "plugin_callurl.h"
#include "settings.h"
#include "plugininterface.h"


Q_EXPORT_PLUGIN2(plugin_callurl, PluginCallURL)

PluginCallURL::PluginCallURL():PluginInterface("callurl", "Plugin to call an URL")
{
}

PluginCallURL::~PluginCallURL()
{
	Cron::UnregisterAll(this);
}

void PluginCallURL::CallURL(Bunny * b, QString url)
{
	QByteArray message = "CU " + url.toAscii() + "\n";
	b->SendPacket(MessagePacket(message));
}

void PluginCallURL::OnCron(Bunny * b, QVariant v)
{
	QString url = v.value<QString>();
	CallURL(b, url);
}


bool PluginCallURL::OnRFID(Bunny * b, QByteArray const& tag)
{
	QString url = b->GetPluginSetting(GetName(), QString("RFIDCallURL/%1").arg(QString(tag.toHex())), QString()).toString();
	if(url != "")
	{
		CallURL(b, url);
		return true;
	}
	return false;
}

bool PluginCallURL::OnClick(Bunny * b, PluginInterface::ClickType type)
{
	if (type == PluginInterface::SingleClick) {
		QString url = b->GetPluginSetting(GetName(), "Default/CallURL", "").toString();
		if(url != "") {
			CallURL(b, url);
			return true;
		}
	}
	return false;
}

void PluginCallURL::OnBunnyConnect(Bunny * b)
{
	QMap<QString, QVariant> list = b->GetPluginSetting(GetName(), "Webcasts", QMap<QString, QVariant>()).toMap();
	QMapIterator<QString, QVariant> i(list);
	while (i.hasNext()) {
		i.next();
		QString time = i.key();
		QString url = i.value().toString();
		Cron::RegisterDaily(this, QTime::fromString(time, "hh:mm"), b, QVariant::fromValue(url));
	}
}

void PluginCallURL::OnBunnyDisconnect(Bunny * b)
{
	Cron::UnregisterAllForBunny(this, b);
}


void PluginCallURL::InitApiCalls()
{
	DECLARE_PLUGIN_BUNNY_API_CALL("addrfid(tag,url)", PluginCallURL, Api_AddRFID);
	DECLARE_PLUGIN_BUNNY_API_CALL("removerfid(tag)", PluginCallURL, Api_RemoveRFID);
	DECLARE_PLUGIN_BUNNY_API_CALL("setdefaulturl(url)", PluginCallURL, Api_setDefaultUrl);
	DECLARE_PLUGIN_BUNNY_API_CALL("getdefaulturl()", PluginCallURL, Api_getDefaultUrl);
	DECLARE_PLUGIN_BUNNY_API_CALL("addwebcast(time,url)", PluginCallURL, Api_AddWebcast);
	DECLARE_PLUGIN_BUNNY_API_CALL("removewebcast(time)", PluginCallURL, Api_RemoveWebcast);
	DECLARE_PLUGIN_BUNNY_API_CALL("getwebcastslist()", PluginCallURL, Api_ListWebcast);
	DECLARE_PLUGIN_BUNNY_API_CALL("addurl(url)", PluginCallURL, Api_addUrl);
	DECLARE_PLUGIN_BUNNY_API_CALL("removeurl(url)", PluginCallURL, Api_removeUrl);
	DECLARE_PLUGIN_BUNNY_API_CALL("geturlslist()", PluginCallURL, Api_getUrlsList);
}

PLUGIN_BUNNY_API_CALL(PluginCallURL::Api_setDefaultUrl)
{
	Q_UNUSED(account);

	if(!hRequest.HasArg("url"))
		return new ApiManager::ApiError(QString("Missing argument 'url' for plugin CallURL"));

	bunny->SetPluginSetting(GetName(), "Default/CallURL", hRequest.GetArg("url"));
	return new ApiManager::ApiOk(QString("New default url defined '%1' for bunny '%2'").arg(hRequest.GetArg("url"), QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginCallURL::Api_getDefaultUrl)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);

	return new ApiManager::ApiString(bunny->GetPluginSetting(GetName(), "Default/CallURL",QString()).toString());
}

PLUGIN_BUNNY_API_CALL(PluginCallURL::Api_AddWebcast)
{
	Q_UNUSED(account);

	QString hTime = hRequest.GetArg("time");
	QString url = hRequest.GetArg("url");
	QMap<QString, QVariant> list = bunny->GetPluginSetting(GetName(), "Webcasts", QMap<QString, QVariant>()).toMap();
	if(!list.contains(hTime))
	{
		Cron::RegisterDaily(this, QTime::fromString(hTime, "hh:mm"), bunny, QVariant::fromValue(url));
		list.insert(hTime,url);
		bunny->SetPluginSetting(GetName(), "Webcasts", list);
		return new ApiManager::ApiOk(QString("Add webcast at '%1' to bunny '%2'").arg(hTime, QString(bunny->GetID())));
	}
	return new ApiManager::ApiError(QString("Webcast already exists at '%1' for bunny '%2'").arg(hTime, QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginCallURL::Api_RemoveWebcast)
{
	Q_UNUSED(account);

	if(!hRequest.HasArg("time"))
		return new ApiManager::ApiError(QString("Missing argument 'time' for plugin CallURL"));

	QMap<QString, QVariant> list = bunny->GetPluginSetting(GetName(), "Webcasts", QMap<QString, QVariant>()).toMap();
	QString time = hRequest.GetArg("time");
    if(list.contains(time))
    {
		list.remove(time);
        bunny->SetPluginSetting(GetName(), "Webcasts", list);

		// Recreate crons
        OnBunnyDisconnect(bunny);
        OnBunnyConnect(bunny);
        return new ApiManager::ApiOk(QString("Remove webcast at '%1' for bunny '%2'").arg(hRequest.GetArg("time"), QString(bunny->GetID())));
    }
    return new ApiManager::ApiError(QString("No webcast at '%1' for bunny '%2'").arg(hRequest.GetArg("time"), QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginCallURL::Api_ListWebcast)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);
	return new ApiManager::ApiMappedList(bunny->GetPluginSetting(GetName(), "Webcasts", QMap<QString, QVariant>()).toMap());
}

PLUGIN_BUNNY_API_CALL(PluginCallURL::Api_AddRFID)
{
	Q_UNUSED(account);

	bunny->SetPluginSetting(GetName(), QString("RFIDCallURL/%1").arg(hRequest.GetArg("tag")), hRequest.GetArg("url"));

	return new ApiManager::ApiOk(QString("Add weather for '%1' for RFID '%2', bunny '%3'").arg(hRequest.GetArg("url"), hRequest.GetArg("tag"), QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginCallURL::Api_RemoveRFID)
{
	Q_UNUSED(account);

	bunny->RemovePluginSetting(GetName(), QString("RFIDCallURL/%1").arg(hRequest.GetArg("tag")));

	return new ApiManager::ApiOk(QString("Remove RFID '%2' for bunny '%3'").arg(hRequest.GetArg("tag"), QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginCallURL::Api_addUrl) {
	Q_UNUSED(account);

	if(!hRequest.HasArg("url"))
		return new ApiManager::ApiError(QString("Missing argument 'url' for plugin CallURL"));
	QString url = hRequest.GetArg("url");
	QStringList list = bunny->GetPluginSetting(GetName(), "Urls", QStringList()).toStringList();
	list.append(url);
	bunny->SetPluginSetting(GetName(), "Urls", list);

	return new ApiManager::ApiOk(QString("Added url '%1' for bunny '%2'").arg(url, QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginCallURL::Api_removeUrl)
{
	Q_UNUSED(account);

	if(!hRequest.HasArg("url"))
		return new ApiManager::ApiError(QString("Missing argument 'url' for plugin CallURL"));

	QString url = hRequest.GetArg("url");
	QStringList list = bunny->GetPluginSetting(GetName(), "Urls", QStringList()).toStringList();
	list.removeAll(url);
	bunny->SetPluginSetting(GetName(), "Urls", list);

	return new ApiManager::ApiOk(QString("Removed url '%1' for bunny '%2'").arg(url, QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginCallURL::Api_getUrlsList) {
	Q_UNUSED(account);
	Q_UNUSED(hRequest);
	return new ApiManager::ApiList(bunny->GetPluginSetting(GetName(), "Urls", QStringList()).toStringList());
}

