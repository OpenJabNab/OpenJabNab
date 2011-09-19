#include <QDateTime>
#include <QStringList>
#include "plugin_rfid.h"
#include "account.h"
#include "accountmanager.h"
#include "bunny.h"
#include "bunnymanager.h"
#include "ztamp.h"
#include "ztampmanager.h"
#include "log.h"
#include "settings.h"

Q_EXPORT_PLUGIN2(plugin_rfid, PluginRFID)

PluginRFID::PluginRFID():PluginInterface("rfid", "Manage RFID requests", SystemPlugin) {}

bool PluginRFID::HttpRequestHandle(HTTPRequest & request)
{
	QString uri = request.GetURI();
	if (uri.startsWith("/vl/rfid.jsp"))
	{
		QString serialnumber = request.GetArg("sn");
		QString tagId = request.GetArg("t");
		SetSettings("global/LastTag", tagId);

		Ztamp * z = ZtampManager::GetZtamp(this, tagId.toAscii());
		Bunny * b = BunnyManager::GetBunny(this, serialnumber.toAscii());
		b->SetPluginSetting(GetName(), "LastTag", tagId);
		/* Get Owner of the bunny */
		QString Bac = b->GetGlobalSetting("OwnerAccount","").toString();
		if(Bac != "") {
			/* Get Owners of the Ztamp */
			QStringList Zac = z->GetGlobalSetting("OwnerAccounts","").toStringList();
			/* None, add it to this account */
			if(!Zac.contains(Bac)) {
				Account *Ac = AccountManager::GetAccountByLogin(Bac.toAscii());
				Ac->AddZtamp(tagId.toAscii());
				Zac.append(Bac);
				z->SetGlobalSetting("OwnerAccounts",Zac);
				LogWarning(QString("Ztamp: %1 added to account %2 by bunny %3").arg(tagId,Bac,serialnumber));
			}
		}

		if (z->OnRFID(b))
			return true;
		if (b->OnRFID(QByteArray::fromHex(tagId.toAscii())))
			return true;
	}
	return false;
}

/*******/
/* API */
/*******/
void PluginRFID::InitApiCalls()
{
	DECLARE_PLUGIN_API_CALL("getLastTag()", PluginRFID, Api_GetLastTag);
	DECLARE_PLUGIN_API_CALL("getLastTagForBunny(sn)", PluginRFID, Api_GetLastTagForBunny);
}

PLUGIN_API_CALL(PluginRFID::Api_GetLastTag)
{
	Q_UNUSED(hRequest);

	if(!account.IsAdmin())
		return new ApiManager::ApiError("Access denied");

	return new ApiManager::ApiString(GetSettings("global/LastTag", QString()).toString());
}

PLUGIN_API_CALL(PluginRFID::Api_GetLastTagForBunny)
{
	Bunny * b = BunnyManager::GetBunny(this, hRequest.GetArg("sn").toAscii());
	if(!account.IsAdmin())
		return new ApiManager::ApiError("Access denied");

	return new ApiManager::ApiString(b->GetPluginSetting(GetName(), "LastTag", QString()).toString());
}

