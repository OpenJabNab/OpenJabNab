#include <QHttp>
#include "bunny.h"
#include "account.h"
#include "plugin_annuaire.h"

Q_EXPORT_PLUGIN2(plugin_annuaire, PluginAnnuaire)

PluginAnnuaire::PluginAnnuaire():PluginInterface("annuaire", "Register the bunny on the central directory", SystemPlugin)
{
}

PluginAnnuaire::~PluginAnnuaire() {}

void PluginAnnuaire::OnBunnyConnect(Bunny * b)
{
	QString server = GetSettings("global/URL", "").toString();
	if(server != "") {
		QHttp *http = new QHttp(server,80);
		http->get("/nabconnection.php?m=" + b->GetID() + "&n="+ b->GetBunnyName() + "&s=" + GlobalSettings::GetString("OpenJabNabServers/PingServer"));
	}
}

/*******/
/* API */
/*******/
void PluginAnnuaire::InitApiCalls()
{
	DECLARE_PLUGIN_API_CALL("setURL(url)", PluginAnnuaire, Api_setURL);
	DECLARE_PLUGIN_API_CALL("getURL()", PluginAnnuaire, Api_getURL);
}

PLUGIN_API_CALL(PluginAnnuaire::Api_setURL)
{
	if(!account.IsAdmin())
		return new ApiManager::ApiError("Access denied");

	QString url = hRequest.GetArg("url");
	SetSettings("global/URL", url);

	return new ApiManager::ApiOk(QString("URL set to '%1'.").arg(url));
}

PLUGIN_API_CALL(PluginAnnuaire::Api_getURL)
{
	Q_UNUSED(hRequest);

	if(!account.IsAdmin())
		return new ApiManager::ApiError("Access denied");

	return new ApiManager::ApiString(QString("%1").arg(GetSettings("global/URL", "").toString()));
}
