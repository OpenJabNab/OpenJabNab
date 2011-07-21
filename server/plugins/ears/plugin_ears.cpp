#include "plugin_ears.h"
#include "bunny.h"
#include "bunnymanager.h"
#include "ambientpacket.h"
#include "messagepacket.h"
Q_EXPORT_PLUGIN2(plugin_ears, PluginEars)

PluginEars::PluginEars():PluginInterface("ears", "Ears Pairing with another Bunny",BunnyPlugin) { }

PluginEars::~PluginEars() {}

bool PluginEars::OnEarsMove(Bunny * b, int l, int r) {
	/* Get Setting */
	QByteArray Friend = b->GetPluginSetting(GetName(), "Friend", "").toByteArray();
	QString FName(Friend);
	/* If setting set */
	if(!Friend.isEmpty()) {
		Bunny *f = BunnyManager::GetBunny(this, Friend);
			/* If aFriend is found */
			if(f) {
				/* If this bunny is the Friend's friend */
				if(f->GetPluginSetting(GetName(), "Friend", "").toByteArray() == b->GetID()) {
					/* If the friend is connected and not sleeping */
					if(f->IsConnected() && !f->IsSleeping()) {
						/* Debug
					    LogDebug(QString("Friend: %1 Ears to: %2 - %3").arg(FName).arg(l).arg(r)); */
						/* Send A packet to move the ears */
						AmbientPacket p;
						p.SetEarsPosition(l, r);
						f->SendPacket(p);
					}
				}
			}
	}

	return true;
}

/*******/
/* API */
/*******/
void PluginEars::InitApiCalls()
{
	/* Basic API calls, Set and Get Friend's ID */
	DECLARE_PLUGIN_BUNNY_API_CALL("getFriend()", PluginEars, Api_getFriend);
	DECLARE_PLUGIN_BUNNY_API_CALL("setFriend(id)", PluginEars, Api_setFriend);
}

PLUGIN_BUNNY_API_CALL(PluginEars::Api_setFriend)
{
	Q_UNUSED(account);
	/* Update Configuration */
	bunny->SetPluginSetting(GetName(), "Friend", QVariant(hRequest.GetArg("id")));
	return new ApiManager::ApiOk(QString("Plugin configuration updated."));
}


PLUGIN_BUNNY_API_CALL(PluginEars::Api_getFriend)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);
	/* Get Configuration */
	return new ApiManager::ApiString(bunny->GetPluginSetting(GetName(), "Friend", "").toString());
}
