#include "plugin_msgall.h"
#include "bunny.h"
#include "account.h"
#include "bunnymanager.h"
#include "ttsmanager.h"
#include "messagepacket.h"

Q_EXPORT_PLUGIN2(plugin_msgall, PluginMsgall)

PluginMsgall::PluginMsgall():PluginInterface("msgall", "Send a message to all the bunnies connected on the server",SystemPlugin)
{
}

PluginMsgall::~PluginMsgall() {}

/*******
 * API *
 *******/

void PluginMsgall::InitApiCalls()
{
	DECLARE_PLUGIN_API_CALL("say(text)", PluginMsgall, Api_Say);
}

PLUGIN_API_CALL(PluginMsgall::Api_Say)
{
	if(!account.IsAdmin())
		return new ApiManager::ApiError("Access denied.");

	QByteArray fileName = TTSManager::CreateNewSound(hRequest.GetArg("text"), "Claire");

	QList<QByteArray> BList = BunnyManager::GetConnectedBunniesList();
	foreach (QByteArray BID, BList)
	{
		Bunny *b = BunnyManager::GetBunny(this, BID);
		b->SendPacket(MessagePacket("MU " + fileName + "\nMW\n"));
	}

	return new ApiManager::ApiOk("Message sent.");
}
