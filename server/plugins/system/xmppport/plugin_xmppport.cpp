#include "plugin_xmppport.h"

Q_EXPORT_PLUGIN2(plugin_xmppport, PluginXmppport)

PluginXmppport::PluginXmppport():PluginInterface("xmppport", "XMPP Port changer in bootcode", RequiredPlugin)
{
}

PluginXmppport::~PluginXmppport() {}

void PluginXmppport::patchBootcode(HTTPRequest & request, long address, int size, QByteArray origin, QByteArray patch)
{
	if(request.reply.indexOf(origin, address) != address)
	{
		LogDebug("Firmware can't be patched");
	}
	else
	{
		LogDebug("Patching firmware");
		request.reply.replace(address, size, patch);
	}
}

void PluginXmppport::HttpRequestAfter(HTTPRequest & request)
{
	QString uri = request.GetURI();
	if (uri.startsWith("/vl/bc.jsp") && GlobalSettings::GetInt("OpenJabNabServers/XmppPort", 5222) != 5222)
	{
		QByteArray port = convertLSBFirst(GlobalSettings::GetInt("OpenJabNabServers/XmppPort", 5222));
		QByteArray origin = convertLSBFirst(5222);
		patchBootcode(request, 0x000111CF, 4, origin, port); 
	}
}

QByteArray PluginXmppport::convertLSBFirst(int i)
{
	QString r = "";
	QString f = "";
	r.setNum(i, 16);
	f.fill('0', 8 - r.size());
	r = f + r;
	r = r.mid(6, 2) + r.mid(4, 2) + r.mid(2, 2) + r.mid(0, 2);
	return QByteArray::fromHex(r.toAscii());
}
