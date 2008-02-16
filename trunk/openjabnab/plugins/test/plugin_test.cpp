#include "ambientpacket.h"
#include "bunny.h"
#include "plugin_test.h"

Q_EXPORT_PLUGIN2(plugin_test, PluginTest)

PluginTest::PluginTest():PluginInterface("test")
{
	angle = 0;
}

bool PluginTest::OnClick(Bunny * b, PluginInterface::ClickType)
{
	angle++;
	AmbientPacket p;
	p.SetEarsPosition(angle%18, 0);
	b->SendPacket(p);
	return true;
}
