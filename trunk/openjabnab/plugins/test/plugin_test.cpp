#include "plugin_test.h"
#include "ambientpacket.h"
#include "bunny.h"
#include "choregraphy.h"
#include "httprequest.h"
#include "messagepacket.h"

Q_EXPORT_PLUGIN2(plugin_test, PluginTest)

PluginTest::PluginTest():PluginInterface("test", "Test choregraphy generation")
{
	angle = 0;
}

PluginTest::~PluginTest()
{
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
		c.SetTempo(400);
		for (int i = 0; i < 48; i++)
		{
			c.AddLedAction(i, Choregraphy::Led_Left, (i%3 == 0)?255:0, 0, 0);
			c.AddLedAction(i, Choregraphy::Led_Middle, 0, (i%3 == 1)?255:0, 0);
			c.AddLedAction(i, Choregraphy::Led_Right, 0, 0, (i%3 == 2)?255:0);
		}
		for (int i = 0; i < 12; i++)
		{
			c.AddMotorAction(4*i, Choregraphy::Ear_Left, 36*i, Choregraphy::Dir_Forward);
			c.AddMotorAction(4*i+2, Choregraphy::Ear_Right, 36*i, Choregraphy::Dir_Forward);
		}
		request.reply = c.GetData();
		return true;
	}
	return false;
}

