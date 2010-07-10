#include <QDateTime>
#include <QStringList>
#include "plugin_rfid.h"
#include "bunny.h"
#include "bunnymanager.h"
#include "log.h"
#include "settings.h"

Q_EXPORT_PLUGIN2(plugin_rfid, PluginRFID)

PluginRFID::PluginRFID():PluginInterface("rfid", "Manage RFID requests", RequiredPlugin) {}

bool PluginRFID::HttpRequestHandle(HTTPRequest & request)
{
	QString uri = request.GetURI();
	if (uri.startsWith("/vl/rfid.jsp"))
	{
		QString serialnumber = request.GetArg("sn");
		QString tagId = request.GetArg("t");

		Bunny * b = BunnyManager::GetBunny(this, serialnumber.toAscii());
		b->SetGlobalSetting("Last RFID Tag", tagId);
		QString tagName=b->GetGlobalSetting(QString("KnownRFID/%1").arg(tagId), QString()).toString();
		if(tagName == "")
		{
			b->SetGlobalSetting(QString("KnownRFID/%1").arg(tagId), QString());
		}
	
		if (b->OnRFID(QByteArray::fromHex(tagId.toAscii())))
			return true;
	}
	return false;
}

void PluginRFID::InitApiCalls()
{
	DECLARE_PLUGIN_BUNNY_API_CALL("namerfid", PluginRFID, Api_NameRFID);
}

PLUGIN_BUNNY_API_CALL(PluginRFID::Api_NameRFID)
{
	Q_UNUSED(account);

	if(!hRequest.HasArg("name"))
		return new ApiManager::ApiError(QString("Missing argument 'name' for plugin RFID"));

	if(!hRequest.HasArg("tag"))
		return new ApiManager::ApiError(QString("Missing argument 'tag' for plugin RFID"));

	if(!bunny->IsConnected())
		return new ApiManager::ApiError(QString("Bunny '%1' is not connected").arg(hRequest.GetArg("to")));

	bunny->SetGlobalSetting(QString("KnownRFID/%1").arg(hRequest.GetArg("tag")), hRequest.GetArg("name"));

	return new ApiManager::ApiString(QString("RFID '%2' is now named '%1' for bunny '%3'").arg(hRequest.GetArg("name"), hRequest.GetArg("tag"), QString(bunny->GetID())));
}

