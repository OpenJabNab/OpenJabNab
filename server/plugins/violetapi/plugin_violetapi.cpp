#include "plugin_violetapi.h"
#include <QCryptographicHash>
#include <QUuid>
#include "bunny.h"
#include "account.h"

Q_EXPORT_PLUGIN2(plugin_violetapi, PluginVioletapi)

PluginVioletapi::PluginVioletapi():PluginInterface("violetapi", "Enable Violet API calls for this bunny",BunnyZtampPlugin)
{
}

PluginVioletapi::~PluginVioletapi() {}

/*******/
/* API */
/*******/
void PluginVioletapi::InitApiCalls()
{
	/* Basic API calls, Set and Get Friend's ID */
	DECLARE_PLUGIN_BUNNY_API_CALL("enable()", PluginVioletapi, Api_enableVApi);
	DECLARE_PLUGIN_BUNNY_API_CALL("disable()", PluginVioletapi, Api_disableVApi);
	DECLARE_PLUGIN_BUNNY_API_CALL("getStatus()", PluginVioletapi, Api_getStatus);
	DECLARE_PLUGIN_BUNNY_API_CALL("getToken()", PluginVioletapi, Api_getToken);
	DECLARE_PLUGIN_BUNNY_API_CALL("setToken(tk)", PluginVioletapi, Api_setToken);
}

PLUGIN_BUNNY_API_CALL(PluginVioletapi::Api_enableVApi)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);
	/* Get Token if exists */
	QString Token = bunny->GetPluginSetting(GetName(), "VApiToken", "").toString();
	if(Token == "") {
		/* Generate random token */
		QByteArray Token = QCryptographicHash::hash(QUuid::createUuid().toString().toAscii(), QCryptographicHash::Md5).toHex();
		bunny->SetPluginSetting(GetName(),"VApiToken",Token);
	}
	bunny->SetPluginSetting(GetName(),"VApiEnable",true);
	return new ApiManager::ApiOk(QString("VioletAPI enabled"));
}


PLUGIN_BUNNY_API_CALL(PluginVioletapi::Api_disableVApi)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);
	bunny->SetPluginSetting(GetName(),"VApiEnable",false);
	return new ApiManager::ApiOk(QString("VioletAPI disabled"));
}

PLUGIN_BUNNY_API_CALL(PluginVioletapi::Api_getStatus)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);
	return new ApiManager::ApiString(bunny->GetPluginSetting(GetName(), "VApiEnable", "false").toString());
}


PLUGIN_BUNNY_API_CALL(PluginVioletapi::Api_getToken)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);
	return new ApiManager::ApiString(bunny->GetPluginSetting(GetName(), "VApiToken", "").toString());
}

PLUGIN_BUNNY_API_CALL(PluginVioletapi::Api_setToken)
{
	if(!account.IsAdmin())
		return new ApiManager::ApiError("Access denied");

	bunny->SetPluginSetting(GetName(),"VApiToken",hRequest.GetArg("tk").toAscii());
	return new ApiManager::ApiOk(QString("VioletAPI Token updated."));
}
