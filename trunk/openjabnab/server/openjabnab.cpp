#include <QTcpSocket>
#include <QString>

#include "openjabnab.h"
#include "accountmanager.h"
#include "bunnymanager.h"
#include "httphandler.h"
#include "log.h"
#include "pluginmanager.h"
#include "settings.h"
#include "xmpphandler.h"

OpenJabNab::OpenJabNab(int argc, char ** argv):QCoreApplication(argc, argv)
{
	GlobalSettings::Init();
	Log::Init();
	Log::Info("-- OpenJabNab Start --");
	PluginManager::Init();
	AccountManager::Init();

	if(GlobalSettings::Get("Config/HttpListener", true) == true)
	{
		// Create Listeners
		httpListener = new QTcpServer(this);
		httpListener->listen(QHostAddress::LocalHost, GlobalSettings::GetInt("OpenJabNabServers/ListeningHttpPort", 8080));
		connect(httpListener, SIGNAL(newConnection()), this, SLOT(NewHTTPConnection()));
	}
	else
		Log::Warning("Warning : HTTP Listener is disabled !");

	if(GlobalSettings::Get("Config/XmppListener", true) == true)
	{
		xmppListener = new QTcpServer(this);
		xmppListener->listen(QHostAddress::Any, GlobalSettings::GetInt("OpenJabNabServers/XmppPort", 5222));
		connect(xmppListener, SIGNAL(newConnection()), this, SLOT(NewXMPPConnection()));
	}
	else
		Log::Warning("Warning : XMPP Listener is disabled !");

	httpApi = GlobalSettings::Get("Config/HttpApi", true).toBool();
	httpViolet = GlobalSettings::Get("Config/HttpViolet", true).toBool();
	Log::Info(QString("Parsing of HTTP Api is ").append((httpApi == true)?"enabled":"disabled"));
	Log::Info(QString("Parsing of HTTP Bunny messages is ").append((httpViolet == true)?"enabled":"disabled"));
}

void OpenJabNab::Close()
{
	emit Quit();
}

OpenJabNab::~OpenJabNab()
{
	Log::Info("-- OpenJabNab Close --");
	xmppListener->close();
	httpListener->close();
	AccountManager::Close();
	PluginManager::Close();
	BunnyManager::Close();
	Log::Close();
	GlobalSettings::Close();
}

void OpenJabNab::NewHTTPConnection()
{
	HttpHandler * h = new HttpHandler(httpListener->nextPendingConnection(), httpApi, httpViolet);
	connect(this, SIGNAL(Quit()), h, SLOT(Disconnect()));
}

void OpenJabNab::NewXMPPConnection()
{
	XmppHandler * x = new XmppHandler(xmppListener->nextPendingConnection());
	connect(this, SIGNAL(Quit()), x, SLOT(Disconnect()));
}
