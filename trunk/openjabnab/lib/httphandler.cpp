#include "apimanager.h"
#include "bunny.h"
#include "bunnymanager.h"
#include "httphandler.h"
#include "httprequest.h"
#include "openjabnab.h"
#include "log.h"
#include "settings.h"

HttpHandler::HttpHandler(QTcpSocket * s, PluginManager * p, ApiManager * a)
{
	incomingHttpSocket = s;
	pluginManager = p;
	apiManager = a;
	connect(s, SIGNAL(readyRead()), this, SLOT(HandleBunnyHTTPRequest()));
}

HttpHandler::~HttpHandler() {}

void HttpHandler::HandleBunnyHTTPRequest()
{
	QByteArray dataIn = incomingHttpSocket->readAll();
	
	if (dataIn.startsWith("/ojn_api/"))
	{
		ApiManager::ApiAnswer * answer = apiManager->ProcessApiCall(dataIn.mid(9));
		incomingHttpSocket->write(answer->GetData());
		delete answer;
	}
	else if (dataIn.startsWith("/vl/rfid.jsp"))
	{
		HTTPRequest request(dataIn);
		QString serialnumber;
		QString tagId;

		foreach(QString arg, request.GetArgs())
		{
			if (arg.startsWith("sn="))
				serialnumber = arg.mid(3);
			else if (arg.startsWith("t="))
				tagId = arg.mid(2);
		}

		Bunny * b = BunnyManager::GetBunny(serialnumber.toAscii());
		b->SetGlobalSetting("Last RFID Tag", tagId);
		
		if (!pluginManager->OnRFID(b, QByteArray::fromHex(tagId.toAscii())))
		{
			// Forward it to Violet's servers
			request.reply = request.ForwardTo(GlobalSettings::GetString("DefaultVioletServers/PingServer"));
			incomingHttpSocket->write(request.reply);
		}
	}
	else
	{
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
	}
	incomingHttpSocket->close();
	delete this;
}
