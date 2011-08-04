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

HttpHandler::HttpHandler(QTcpSocket * s, bool api, bool violetapi):pluginManager(PluginManager::Instance())
{
	incomingHttpSocket = s;
	httpApi = api;
	httpVioletApi = violetapi;
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
	else if (uri.startsWith("/ojn/FR/api"))
	{
		NetworkDump::Log("Violet Api Call", request.GetRawURI());
		if(httpVioletApi)
		{
			std::auto_ptr<ApiManager::ApiAnswer> apianswer(ApiManager::Instance().ProcessApiCall(uri, request));
			QByteArray answer = apianswer->GetData();
			incomingHttpSocket->write(answer);
			NetworkDump::Log("Violet Api Answer", answer);
		}
		else
			incomingHttpSocket->write("Violet Api is disabled");
	}
	else
	{
		NetworkDump::Log("HTTP Request", request.GetRawURI());
		pluginManager.HttpRequestBefore(request);
		if (!pluginManager.HttpRequestHandle(request))
		{
			LogError(QString("Unable to handle HTTP Request : ") + request.toString());
			request.reply = "404 Not Found";
		}
		pluginManager.HttpRequestAfter(request);
		incomingHttpSocket->write(request.reply);
		if(!uri.contains("itmode.jsp") && !uri.contains(".mp3") && !uri.contains(".chor") && !uri.contains("bc.jsp") && request.reply.size() < 256) // Don't dump too big answers
			NetworkDump::Log("HTTP Answer", request.reply);
	}
	Disconnect();
}

void HttpHandler::Disconnect()
{
	incomingHttpSocket->disconnectFromHost();
	// Delete incomingHttpSocket when it will be disconnected
	connect(incomingHttpSocket, SIGNAL(disconnected()), incomingHttpSocket, SLOT(deleteLater()));
	deleteLater();
}
