#include <QRegExp>
#include "plugin_removesig.h"
#include "bunny.h"
#include "log.h"
#include "messagepacket.h"
#include "packet.h"
#include "settings.h"

Q_EXPORT_PLUGIN2(plugin_removesig, PluginRemoveSig)

PluginRemoveSig::PluginRemoveSig():PluginInterface("removesig", "Removes signature from ambient message")
{
}

bool PluginRemoveSig::XmppVioletPacketMessage(Bunny * b, Packet const& p)
{
	QRegExp rx("MC broadcast/[a-z_/]*signature.mp3\nCL \\d+\nCH broadcast/[a-z_/]*\\.chor\nMW\n");
	if(QString(p.GetPrintableData()).indexOf(rx) != -1)
	{
		QString packet = p.GetPrintableData();
		Log::Debug(QString("Before removing signature :\n%1").arg(packet));
		packet = packet.remove(rx);
		Log::Debug(QString("After removing signature :\n%1").arg(packet));
		b->SendPacket(MessagePacket(packet.toAscii()));
		return true;
	}
	return false;
}

PluginRemoveSig::~PluginRemoveSig()
{
}
