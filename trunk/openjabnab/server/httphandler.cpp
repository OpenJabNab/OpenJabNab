#include "httphandler.h"
#include "openjabnab.h"
#include "httprequest.h"
#include "settings.h"
#include "log.h"

HttpHandler::HttpHandler(QTcpSocket * s)
{
	this->incomingHttpSocket = s;
	connect(s, SIGNAL(readyRead()), this, SLOT(handleBunnyHTTPRequest()));
}

HttpHandler::~HttpHandler()
{
}

void HttpHandler::handleBunnyHTTPRequest()
{
	QByteArray dataIn = incomingHttpSocket->readAll();
	bool handled = false;
	
	HTTPRequest request(dataIn);
	
	Log::Debug(QString("HTTP REQUEST : ") + dataIn);
	
	QVector<PluginInterface *> plugins = OpenJabNab::GetPlugins();
	// Call RequestBefore for all plugins
	foreach(PluginInterface * plugin, plugins)
	{
		plugin->HttpRequestBefore(request);
	}
	
	// Call GetAnswer for all plugins until one returns true
	foreach(PluginInterface * plugin, plugins)
	{
		handled = plugin->HttpRequestHandle(request);
		if(handled)
			break;
	}
	
	// If none can answer, try to forward directly to Violet's servers
	if (!handled)
	{
		if (dataIn.startsWith("/vl/sendMailXMPP.jsp"))
		{
			Log::Warning("XMPP-Bosh not yet supported !");
			request.reply = "XMPP-Bosh not yet supported !";
		}
		else if (dataIn.startsWith("/vl/"))
			request.reply = request.ForwardTo(GlobalSettings::GetString("DefaultVioletServers/PingServer"));
		else if (dataIn.startsWith("/broad/"))
			request.reply = request.ForwardTo(GlobalSettings::GetString("DefaultVioletServers/BroadServer"));
		else
		{
			Log::Error(QString("Unable to handle HTTP Request : ") + dataIn);
			request.reply = "404 Not Found\n";
		}
	}
	
	// Call RequestAfter for all plugins
	foreach(PluginInterface * plugin, plugins)
	{
		plugin->HttpRequestAfter(request);
	}
	
	incomingHttpSocket->write(request.reply);
	Log::Debug(QString("HTTP ANSWER : ") + request.reply);
	incomingHttpSocket->close();
	delete this;
}
