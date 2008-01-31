#include "httphandler.h"
#include "openjabnab.h"
#include "httprequest.h"
#include "settings.h"
#include "log.h"

HttpHandler::HttpHandler(QTcpSocket * s, PluginManager * p)
{
	this->incomingHttpSocket = s;
	this->pluginManager = p;
	connect(s, SIGNAL(readyRead()), this, SLOT(handleBunnyHTTPRequest()));
}

HttpHandler::~HttpHandler()
{
}

void HttpHandler::handleBunnyHTTPRequest()
{
	QByteArray dataIn = incomingHttpSocket->readAll();
	
	HTTPRequest request(dataIn);
	
	Log::Debug(QString("HTTP REQUEST : ") + dataIn);
	
	pluginManager->HttpRequestBefore(request);
	
	// If none can answer, try to forward directly to Violet's servers
	if (!pluginManager->HttpRequestHandle(request))
	{
		if (dataIn.startsWith("/vl/sendMailXMPP.jsp"))
		{
			Log::Warning("Problem with the bunny, he's calling sendMailXMPP.jsp !");
			request.reply = "Not Allowed !";
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
	
	pluginManager->HttpRequestAfter(request);
	
	incomingHttpSocket->write(request.reply);
	Log::Debug(QString("HTTP REPLY : ") + request.reply);
	incomingHttpSocket->close();
	delete this;
}
