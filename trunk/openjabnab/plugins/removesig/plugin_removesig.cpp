#include <QRegExp>
#include "plugin_removesig.h"
#include "bunny.h"
#include "log.h"
#include "messagepacket.h"
#include "packet.h"
#include "settings.h"

Q_EXPORT_PLUGIN2(plugin_removesig, PluginRemoveSig)

PluginRemoveSig::PluginRemoveSig():PluginInterface("removesig", "Remove signature from ambient message")
{
}

bool PluginRemoveSig::XmppVioletPacketMessage(Bunny * b, Packet const& p)
{
	QRegExp rx("MC broadcast/[a-z_/]*signature.mp3\nCL \\d+\nCH broadcast/[a-z_/]*\\.chor\nMW\n");
	if(QString(p.GetPrintableData()).indexOf(rx) != -1)
	{
		QString packet = p.GetPrintableData();
		Log::Debug("Before removing signature :\n" + packet);
		packet = packet.remove(rx);
		Log::Debug("After removing signature :\n" + packet);
		b->SendPacket(MessagePacket(packet.toAscii()));
		return true;
	}
	return false;
}

PluginRemoveSig::~PluginRemoveSig()
{
}
