#include "ambientpacket.h"
#include "bunny.h"
#include "choregraphy.h"
#include "httprequest.h"
#include "messagepacket.h"
#include "plugin_test.h"

Q_EXPORT_PLUGIN2(plugin_test, PluginTest)

PluginTest::PluginTest():PluginInterface("test")
{
	angle = 0;
}

bool PluginTest::OnClick(Bunny * b, PluginInterface::ClickType type)
{
	if (type == PluginInterface::SingleClick)
	{
		// AmbientPacket Test
		angle++;
		AmbientPacket p;
		p.SetEarsPosition(angle%18, 0);
		b->SendPacket(p);
	}
	else
	{
		// Chor test
		b->SendPacket(MessagePacket("CH broadcast/openjabnab/plugin_test/chor1.chor\n"));
	}
	return true;
}

bool PluginTest::HttpRequestHandle(HTTPRequest & request)
{
	QString uri = request.GetURI();
	if (uri.startsWith("/openjabnab/plugin_test"))
	{
		Choregraphy c;
		c.SetTempo(500);
		for (int i = 0; i < 10; i++)
			c.AddLedAction(i, Choregraphy::Led_Middle, 0, (i%2 == 1)?0:255, 0);
		request.reply = c.GetData();
		return true;
	}
	return false;
}
