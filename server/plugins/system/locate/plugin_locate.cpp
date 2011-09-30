#include <QDateTime>
#include <QStringList>
#include "plugin_locate.h"
#include "bunny.h"
#include "bunnymanager.h"
#include "log.h"
#include "settings.h"

Q_EXPORT_PLUGIN2(plugin_locate, PluginLocate)

PluginLocate::PluginLocate():PluginInterface("locate", "Manage Locate requests", RequiredPlugin) {}

bool PluginLocate::HttpRequestHandle(HTTPRequest & request)
{
	QString uri = request.GetURI();
	if (uri.startsWith("/vl/locate.jsp"))
	{
		QString serialnumber = request.GetArg("sn").remove(':');
		Bunny * bunny = BunnyManager::GetBunny(this, serialnumber.toAscii());

		QString pingServer = bunny->GetPluginSetting(GetName(), "PingServer", GlobalSettings::GetString("OpenJabNabServers/PingServer")).toString(); 
		QString broadServer = bunny->GetPluginSetting(GetName(), "BroadServer", GlobalSettings::GetString("OpenJabNabServers/BroadServer")).toString(); 
		QString xmppServer = bunny->GetPluginSetting(GetName(), "XmppServer", GlobalSettings::GetString("OpenJabNabServers/XmppServer")).toString(); 
		QString xmppPort = bunny->GetPluginSetting(GetName(), "ListeningXmppPort", GlobalSettings::GetString("OpenJabNabServers/ListeningXmppPort")).toString(); 

		LogInfo(QString("Requesting LOCATE for tag %1").arg(serialnumber));
		
		QString locateString;
		locateString += QString("ping %1\n").arg(pingServer);
		locateString += QString("broad %1\n").arg(broadServer);
		locateString += QString("xmpp_domain %1:%2\n").arg(xmppServer, xmppPort);
		request.reply = locateString.toAscii();

		bunny->SetGlobalSetting("LastLocate", QDateTime::currentDateTime());
		bunny->SetGlobalSetting("LastLocateString", locateString);
		
		return true;
	}
	else
		return false;
}

void PluginLocate::InitApiCalls()
{
	DECLARE_PLUGIN_BUNNY_API_CALL("setcustomlocate(param,value)", PluginLocate, Api_SetCustomLocateSetting);
	DECLARE_PLUGIN_BUNNY_API_CALL("getcustomlocate(param)", PluginLocate, Api_GetCustomLocateSetting);
}

PLUGIN_BUNNY_API_CALL(PluginLocate::Api_SetCustomLocateSetting)
{
	Q_UNUSED(account);

	QString hParam = hRequest.GetArg("param");
	if(hParam == "PingServer" || hParam == "BroadServer" || hParam == "XmppServer" || hParam == "ListeningXmppPort")
	{
		if(hRequest.GetArg("value") != "")
		{
			bunny->SetPluginSetting(GetName(), hParam, hRequest.GetArg("value"));
			return new ApiManager::ApiOk(QString("Setting '%1' to custom value '%2'").arg(hParam, hRequest.GetArg("value")));
		}
		else
		{
			bunny->RemovePluginSetting(GetName(), hParam);
			return new ApiManager::ApiOk(QString("Removing '%1' custom value").arg(hParam));
		}
	}
	return new ApiManager::ApiError(QString("'%1' is not a setting for this plugin").arg(hParam));
}

PLUGIN_BUNNY_API_CALL(PluginLocate::Api_GetCustomLocateSetting)
{
	Q_UNUSED(account);

	QString hParam = hRequest.GetArg("param");
	if(hParam == "PingServer" || hParam == "BroadServer" || hParam == "XmppServer" || hParam == "ListeningXmppPort")
	{
		return new ApiManager::ApiString(bunny->GetPluginSetting(GetName(), hParam, QString("")).toString());
	}
	return new ApiManager::ApiError(QString("'%1' is not a setting for this plugin").arg(hParam));
}

