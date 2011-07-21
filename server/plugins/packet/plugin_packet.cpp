#include "plugin_packet.h"
#include "bunny.h"
#include "messagepacket.h"

Q_EXPORT_PLUGIN2(plugin_packet, PluginPacket)

PluginPacket::PluginPacket():PluginInterface("packet", "Send raw packets to bunny",BunnyPlugin) {}

void PluginPacket::InitApiCalls()
{
	DECLARE_PLUGIN_BUNNY_API_CALL("sendPacket(data)", PluginPacket, Api_SendPacket);
	DECLARE_PLUGIN_BUNNY_API_CALL("sendMessage(msg)", PluginPacket, Api_SendMessage);
}

PLUGIN_BUNNY_API_CALL(PluginPacket::Api_SendPacket)
{
	Q_UNUSED(account);

	QByteArray data = QByteArray::fromHex(hRequest.GetArg("data").toAscii());
	bunny->SendData(data);
	return new ApiManager::ApiOk(QString("'%1' sent to bunny").arg(QString(data.toHex())));
}

PLUGIN_BUNNY_API_CALL(PluginPacket::Api_SendMessage)
{
	Q_UNUSED(account);

	QByteArray msg = hRequest.GetArg("msg").toAscii();
	bunny->SendPacket(MessagePacket(msg));
	return new ApiManager::ApiOk(QString("'%1' sent to bunny").arg(QString(msg)));
}
