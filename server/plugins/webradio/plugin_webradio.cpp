#include <QMap>
#include <QMapIterator>
#include <QTime>
#include "plugin_webradio.h"
#include "bunny.h"
#include "cron.h"
#include "bunnymanager.h"
#include "messagepacket.h"

Q_EXPORT_PLUGIN2(plugin_webradio, PluginWebradio)

PluginWebradio::PluginWebradio():PluginInterface("webradio", "WebRadio", BunnyZtampPlugin) {}

PluginWebradio::~PluginWebradio()
{
    Cron::UnregisterAll(this);
}

bool PluginWebradio::Init()
{
	return true;
}

void PluginWebradio::OnCron(Bunny * b, QVariant v)
{
	QString name = v.value<QString>();
        streamPresetWebradio(b, name);
}

bool PluginWebradio::OnRFID(Ztamp * z, Bunny * b)
{
	LogInfo(QString("OnRFID zTamp %1 %2").arg(z->GetZtampName(), b->GetBunnyName()));
	QString radio = z->GetPluginSetting(GetName(), QString("Play"), QString()).toString();
	if(radio != "")
	{
		LogInfo(QString("Will now stream from ztamp : %1").arg(radio));
        	return streamPresetWebradio(b, radio);
	}
	return false;
}

bool PluginWebradio::OnRFID(Bunny * b, QByteArray const& tag)
{
	LogInfo(QString("OnRFID bunny %1 %2").arg(b->GetBunnyName(), QString(tag)));
	QString radio = b->GetPluginSetting(GetName(), QString("RFIDPlay/%1").arg(QString(QByteArray::fromHex(tag))), QString()).toString();
	if(radio != "")
	{
		LogInfo(QString("Will now stream : %1").arg(radio));
        	return streamPresetWebradio(b, radio);
	}
	return false;
}

bool PluginWebradio::OnClick(Bunny * b, PluginInterface::ClickType)
{
	QString radio = b->GetPluginSetting(GetName(), "DefaultWebradio", QString()).toString();
	if(radio.length())
	{
		if(streamPresetWebradio(b, radio))
			return true;
		return false;
	}
	return false;
}

void PluginWebradio::OnBunnyConnect(Bunny * b)
{
	QMap<QString, QVariant> list = b->GetPluginSetting(GetName(), "Webcasts", QMap<QString, QVariant>()).toMap();
	QMapIterator<QString, QVariant> i(list);
	while (i.hasNext()) {
		i.next();
		QString time = i.key();
		QString name = i.value().toString();
		Cron::RegisterDaily(this, QTime::fromString(time, "hh:mm"), b, QVariant::fromValue(name));
	}
}

void PluginWebradio::OnBunnyDisconnect(Bunny * b)
{
	Cron::UnregisterAllForBunny(this, b);
}

bool PluginWebradio::streamWebradio(Bunny * b, QString url)
{
	QByteArray message = "ST "+url.toAscii()+"\nPL "+QString::number(qrand() % 8).toAscii()+"\nMW\n";
	b->SendPacket(MessagePacket(message));
	return true;
}

bool PluginWebradio::streamPresetWebradio(Bunny * b, QString preset)
{
	QMap<QString, QVariant> list = b->GetPluginSetting(GetName(), "Presets", QMap<QString, QVariant>()).toMap();
	if(list.contains(preset))
	{
		QString url = list.value(preset).toString();
		return streamWebradio(b, url);
	}
	return false;
}

/*******
 * API *
 *******/

void PluginWebradio::InitApiCalls()
{
	DECLARE_PLUGIN_BUNNY_API_CALL("addrfid(tag,name)", PluginWebradio, Api_AddRFID);
	DECLARE_PLUGIN_BUNNY_API_CALL("removerfid(tag)", PluginWebradio, Api_RemoveRFID);
	DECLARE_PLUGIN_BUNNY_API_CALL("addpreset(name,url)", PluginWebradio, Api_AddPreset);
	DECLARE_PLUGIN_BUNNY_API_CALL("removepreset(name)", PluginWebradio, Api_RemovePreset);
	DECLARE_PLUGIN_BUNNY_API_CALL("addwebcast(time,name)", PluginWebradio, Api_AddWebcast);
	DECLARE_PLUGIN_BUNNY_API_CALL("removewebcast(time)", PluginWebradio, Api_RemoveWebcast);
	DECLARE_PLUGIN_BUNNY_API_CALL("setdefault(name)", PluginWebradio, Api_SetDefault);
	DECLARE_PLUGIN_BUNNY_API_CALL("getdefault()", PluginWebradio, Api_GetDefault);
	DECLARE_PLUGIN_BUNNY_API_CALL("play(name)", PluginWebradio, Api_Play);
	DECLARE_PLUGIN_BUNNY_API_CALL("playurl(url)", PluginWebradio, Api_PlayUrl);
	DECLARE_PLUGIN_BUNNY_API_CALL("listwebcast()", PluginWebradio, Api_ListWebcast);
	DECLARE_PLUGIN_BUNNY_API_CALL("listpreset()", PluginWebradio, Api_ListPreset);
}

PLUGIN_BUNNY_API_CALL(PluginWebradio::Api_AddRFID)
{
	Q_UNUSED(account);

	if(!bunny->IsConnected())
		return new ApiManager::ApiError(QString("Bunny '%1' is not connected").arg(hRequest.GetArg("to")));

	bunny->SetPluginSetting(GetName(), QString("RFIDPlay/%1").arg(hRequest.GetArg("tag")), hRequest.GetArg("name"));

	return new ApiManager::ApiOk(QString("Add '%1' for RFID '%2', bunny '%3'").arg(hRequest.GetArg("name"), hRequest.GetArg("tag"), QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginWebradio::Api_RemoveRFID)
{
	Q_UNUSED(account);

	if(!bunny->IsConnected())
		return new ApiManager::ApiError(QString("Bunny '%1' is not connected").arg(hRequest.GetArg("to")));

	bunny->RemovePluginSetting(GetName(), QString("RFIDPlay/%1").arg(hRequest.GetArg("tag")));

	return new ApiManager::ApiOk(QString("Remove RFID '%2' for bunny '%3'").arg(hRequest.GetArg("tag"), QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginWebradio::Api_AddPreset)
{
	Q_UNUSED(account);

	if(!bunny->IsConnected())
		return new ApiManager::ApiError(QString("Bunny '%1' is not connected").arg(hRequest.GetArg("to")));

	QMap<QString, QVariant> list = bunny->GetPluginSetting(GetName(), "Presets", QMap<QString, QVariant>()).toMap();
	list.insert(hRequest.GetArg("name"), hRequest.GetArg("url"));
	bunny->SetPluginSetting(GetName(), "Presets", list);

	return new ApiManager::ApiOk(QString("Add preset '%1' for radio '%2', bunny '%3'").arg(hRequest.GetArg("name"), hRequest.GetArg("url"), QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginWebradio::Api_RemovePreset)
{
	Q_UNUSED(account);

	if(!bunny->IsConnected())
		return new ApiManager::ApiError(QString("Bunny '%1' is not connected").arg(hRequest.GetArg("to")));

	QMap<QString, QVariant> list = bunny->GetPluginSetting(GetName(), "Presets", QMap<QString, QVariant>()).toMap();
	list.remove(hRequest.GetArg("name"));
	bunny->SetPluginSetting(GetName(), "Presets", list);

	return new ApiManager::ApiOk(QString("Remove preset '%1' for bunny '%2'").arg(hRequest.GetArg("name"), QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginWebradio::Api_GetDefault)
{
	Q_UNUSED(account);

	if(!bunny->IsConnected())
		return new ApiManager::ApiError(QString("Bunny '%1' is not connected").arg(hRequest.GetArg("to")));

	QString name = bunny->GetPluginSetting(GetName(), "DefaultWebradio", QString()).toString();
	return new ApiManager::ApiString(name);
}

PLUGIN_BUNNY_API_CALL(PluginWebradio::Api_SetDefault)
{
	Q_UNUSED(account);

	if(!bunny->IsConnected())
		return new ApiManager::ApiError(QString("Bunny '%1' is not connected").arg(hRequest.GetArg("to")));

	bunny->SetPluginSetting(GetName(), "DefaultWebradio", hRequest.GetArg("name"));
	return new ApiManager::ApiOk(QString("Define '%1' preset webradio as default for bunny '%2'").arg(hRequest.GetArg("name"), QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginWebradio::Api_Play)
{
	Q_UNUSED(account);

	if(!bunny->IsConnected())
		return new ApiManager::ApiError(QString("Bunny '%1' is not connected").arg(hRequest.GetArg("to")));

    if(streamPresetWebradio(bunny, hRequest.GetArg("name")))
    	return new ApiManager::ApiOk(QString("Now streaming '%1' on bunny '%2'").arg(hRequest.GetArg("name"), QString(bunny->GetID())));
	return new ApiManager::ApiError(QString("Can't stream '%1' on bunny '%2'").arg(hRequest.GetArg("name"), QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginWebradio::Api_PlayUrl)
{
	Q_UNUSED(account);

	if(!bunny->IsConnected())
		return new ApiManager::ApiError(QString("Bunny '%1' is not connected").arg(hRequest.GetArg("to")));

    if(streamWebradio(bunny, hRequest.GetArg("url")))
    	return new ApiManager::ApiOk(QString("Now streaming '%1' on bunny '%2'").arg(hRequest.GetArg("url"), QString(bunny->GetID())));
	return new ApiManager::ApiError(QString("Can't stream '%1' on bunny '%2'").arg(hRequest.GetArg("url"), QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginWebradio::Api_AddWebcast)
{
    Q_UNUSED(account);

    if(!bunny->IsConnected())
        return new ApiManager::ApiError(QString("Bunny '%1' is not connected").arg(hRequest.GetArg("to")));

    QString hTime = hRequest.GetArg("time");
	if(!bunny->GetPluginSetting(GetName(), "Webcasts", QMap<QString, QVariant>()).toMap().contains(hTime))
    {
        Cron::RegisterDaily(this, QTime::fromString(hTime, "hh:mm"), bunny, QVariant::fromValue(hRequest.GetArg("name")));
	QMap<QString, QVariant> list = bunny->GetPluginSetting(GetName(), "Webcasts", QMap<QString, QVariant>()).toMap();
        list.insert(hTime, hRequest.GetArg("name"));
        bunny->SetPluginSetting(GetName(), "Webcasts", list);
        return new ApiManager::ApiOk(QString("Add webcast at '%1' to bunny '%2'").arg(hRequest.GetArg("time"), QString(bunny->GetID())));
    }
    return new ApiManager::ApiError(QString("Webcast at '%1' already exists for bunny '%2'").arg(hRequest.GetArg("time"), QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginWebradio::Api_RemoveWebcast)
{
    Q_UNUSED(account);

    if(!bunny->IsConnected())
        return new ApiManager::ApiError(QString("Bunny '%1' is not connected").arg(QString(bunny->GetID())));

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

PLUGIN_BUNNY_API_CALL(PluginWebradio::Api_ListWebcast)
{
	Q_UNUSED(account);

	if(!bunny->IsConnected())
		return new ApiManager::ApiError(QString("Bunny '%1' is not connected").arg(hRequest.GetArg("to")));

	QMap<QString, QVariant> list = bunny->GetPluginSetting(GetName(), "Webcasts", QMap<QString, QVariant>()).toMap();
	
	return new ApiManager::ApiMappedList(list);
}

PLUGIN_BUNNY_API_CALL(PluginWebradio::Api_ListPreset)
{
	Q_UNUSED(account);

	if(!bunny->IsConnected())
		return new ApiManager::ApiError(QString("Bunny '%1' is not connected").arg(hRequest.GetArg("to")));

	QMap<QString, QVariant> list = bunny->GetPluginSetting(GetName(), "Presets", QMap<QString, QVariant>()).toMap();
	
	return new ApiManager::ApiMappedList(list);
}
