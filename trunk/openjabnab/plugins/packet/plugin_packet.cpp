#include "plugin_packet.h"
#include "bunny.h"
#include "messagepacket.h"

Q_EXPORT_PLUGIN2(plugin_packet, PluginPacket)

PluginPacket::PluginPacket():PluginInterface("packet", "Send raw packets to bunny")
{
}

PluginPacket::~PluginPacket()
{
}

ApiManager::ApiAnswer * PluginPacket::ProcessBunnyApiCall(Bunny * b, Account const&, QString const& funcName, HTTPRequest const& r)
{
	if(funcName == "sendPacket" && r.HasArg("data"))
	{
		QByteArray data = QByteArray::fromBase64(r.GetArg("data").toAscii());
		b->SendData(data);
		return new ApiManager::ApiOk(QString("'%1' sent to bunny").arg(QString(data.toHex())));
	}
	if(funcName == "sendMessage" && r.HasArg("msg"))
	{
		QByteArray msg = r.GetArg("msg").toAscii();
		b->SendPacket(MessagePacket(msg));
		return new ApiManager::ApiOk(QString("'%1' sent to bunny").arg(QString(msg)));
	}
	return new ApiManager::ApiError(QString("Bad API Call"));
}
