#include <QRegExp>
#include "plugin_repeat.h"
#include "bunny.h"
#include "log.h"
#include "messagepacket.h"
#include "packet.h"
#include "settings.h"

Q_EXPORT_PLUGIN2(plugin_repeat, PluginRepeat)

PluginRepeat::PluginRepeat():PluginInterface("repeat", "Repeat last Violet message", BunnyPlugin)
{
}

bool PluginRepeat::XmppVioletPacketMessage(Bunny * b, Packet const& p)
{
        b->SetPluginSetting(GetName(), "LastMessage", p.GetPrintableData());
	return false;
}

bool PluginRepeat::OnClick(Bunny * b, PluginInterface::ClickType type)
{
	if (type == PluginInterface::DoubleClick)
	{
		b->SendPacket(MessagePacket(b->GetPluginSetting(GetName(), "LastMessage").toByteArray()));
		return true;
	}
	return false;
}

PluginRepeat::~PluginRepeat() {}
