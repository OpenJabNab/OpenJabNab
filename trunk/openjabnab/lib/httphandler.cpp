#include <QByteArray>
#include <memory>
#include "apimanager.h"
#include "bunny.h"
#include "bunnymanager.h"
#include "httphandler.h"
#include "httprequest.h"
#include "log.h"
#include "netdump.h"
#include "openjabnab.h"
#include "settings.h"

HttpHandler::HttpHandler(QTcpSocket * s, bool api, bool violet):pluginManager(PluginManager::Instance())
{
	incomingHttpSocket = s;
	httpApi = api;
	httpViolet = violet;
	bytesToReceive = 0;
	connect(s, SIGNAL(readyRead()), this, SLOT(ReceiveData()));
}

HttpHandler::~HttpHandler() {}

void HttpHandler::ReceiveData()
{
	receivedData += incomingHttpSocket->readAll();
	if(bytesToReceive == 0 && (receivedData.size() >= 4))
		bytesToReceive = *(int *)receivedData.left(4).constData();

	if(bytesToReceive != 0 && (receivedData.size() == bytesToReceive))
		HandleBunnyHTTPRequest();
}

void HttpHandler::HandleBunnyHTTPRequest()
{
	HTTPRequest request(receivedData);
	QString uri = request.GetURI();
	if (uri.startsWith("/ojn_api/"))
	{
		NetworkDump::Log("Api Call", request.GetRawURI());
		if(httpApi)
		{
			std::auto_ptr<ApiManager::ApiAnswer> apianswer(ApiManager::Instance().ProcessApiCall(uri.mid(9), request));
			QByteArray answer = apianswer->GetData();
			incomingHttpSocket->write(answer);
			NetworkDump::Log("Api Answer", answer);
		}
		else
			incomingHttpSocket->write("Api is disabled");
	}
	else if(httpViolet)
	{
		NetworkDump::Log("HTTP Request", request.GetRawURI());
		pluginManager.HttpRequestBefore(request);
		// If none can answer, try to forward it directly to Violet's servers
		if (!pluginManager.HttpRequestHandle(request))
		{
			if (uri.startsWith("/vl/sendMailXMPP.jsp"))
			{
				Log::Warning("Problem with the bunny, he's calling sendMailXMPP.jsp !");
				request.reply = "Not Allowed !";
			}
			else if (uri.startsWith("/vl/"))
				request.reply = request.ForwardTo(GlobalSettings::GetString("DefaultVioletServers/PingServer"));
			else if (uri.startsWith("/broad/"))
				request.reply = request.ForwardTo(GlobalSettings::GetString("DefaultVioletServers/BroadServer"));
			else
			{
				Log::Error(QString("Unable to handle HTTP Request : ") + request.toString());
				request.reply = "404 Not Found";
			}
		}
		pluginManager.HttpRequestAfter(request);
		incomingHttpSocket->write(request.reply);
		if(!uri.contains(".mp3") && !uri.contains(".chor") && !uri.contains("bc.jsp") && request.reply.size() < 256) // Don't dump too big answers
			NetworkDump::Log("HTTP Answer", request.reply);
	}
	else
	{
		incomingHttpSocket->write("Bunny's message parsing is disabled <br />");
		incomingHttpSocket->write("Request was : <br />");
		incomingHttpSocket->write(request.toString().toAscii());
	}
	incomingHttpSocket->close();
	deleteLater();
}

void HttpHandler::Disconnect()
{
	incomingHttpSocket->close();
	deleteLater();
}
