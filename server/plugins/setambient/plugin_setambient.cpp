#include "plugin_setambient.h"
#include "ambientpacket.h"
#include "bunny.h"


Q_EXPORT_PLUGIN2(plugin_setambient, PluginSetAmbient)

PluginSetAmbient::PluginSetAmbient():PluginInterface("setambient", "Send Ambient Packet to the bunny", BunnyPlugin)
{
}


/*******
 * API *
 *******/

void PluginSetAmbient::InitApiCalls()
{
	DECLARE_PLUGIN_BUNNY_API_CALL("ambientPacket(service,value)", PluginSetAmbient, Api_SendAmbient);
}

PLUGIN_BUNNY_API_CALL(PluginSetAmbient::Api_SendAmbient)
{
	Q_UNUSED(account);
	if(bunny->IsConnected())
		bunny->SendPacket(AmbientPacket((AmbientPacket::Services)hRequest.GetArg("service").toInt(),(unsigned char)hRequest.GetArg("value").toInt()));
	return new ApiManager::ApiOk("Ambient Packet Sent");
}