#include "httphandler.h"
#include "httprequest.h"
#include "openjabnab.h"
#include "log.h"
#include "settings.h"

HttpHandler::HttpHandler(QTcpSocket * s, PluginManager * p)
{
	this->incomingHttpSocket = s;
	this->pluginManager = p;
	connect(s, SIGNAL(readyRead()), this, SLOT(HandleBunnyHTTPRequest()));
}

HttpHandler::~HttpHandler() {}

void HttpHandler::HandleBunnyHTTPRequest()
{
	QByteArray dataIn = incomingHttpSocket->readAll();
	
	HTTPRequest request(dataIn);
	
	pluginManager->HttpRequestBefore(request);
	
	// If none can answer, try to forward it directly to Violet's servers
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
	incomingHttpSocket->close();
	delete this;
}
