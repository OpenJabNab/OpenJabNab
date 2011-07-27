#include "plugin_xmppport.h"
#include "account.h"

Q_EXPORT_PLUGIN2(plugin_xmppport, PluginXmppport)

PluginXmppport::PluginXmppport():PluginInterface("xmppport", "XMPP Port changer in bootcode", SystemPlugin)
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
	int Cport = GetSettings("global/XmppPort", 5222).toInt();
	if (uri.startsWith("/vl/bc.jsp") && Cport != 5222)
	{
		QByteArray port = convertLSBFirst(Cport);
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

/*******/
/* API */
/*******/
void PluginXmppport::InitApiCalls()
{
	DECLARE_PLUGIN_API_CALL("setXmppPort(port)", PluginXmppport, Api_setXmppPort);
	DECLARE_PLUGIN_API_CALL("getXmppPort()", PluginXmppport, Api_getXmppPort);
}

PLUGIN_API_CALL(PluginXmppport::Api_setXmppPort)
{
	if(!account.IsAdmin())
		return new ApiManager::ApiError("Access denied");

	QString port = hRequest.GetArg("port");
	SetSettings("global/XmppPort", port);

	return new ApiManager::ApiOk(QString("XMPP Port set to '%1'. Change will be effective on next reboot.").arg(port));
}

PLUGIN_API_CALL(PluginXmppport::Api_getXmppPort)
{
	Q_UNUSED(hRequest);

	if(!account.IsAdmin())
		return new ApiManager::ApiError("Access denied");

	return new ApiManager::ApiString(QString("%1").arg(GetSettings("global/XmppPort", 5222).toString()));
}