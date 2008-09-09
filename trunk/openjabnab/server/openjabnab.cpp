#include <QTcpSocket>
#include <QString>

#include "openjabnab.h"
#include "httphandler.h"
#include "xmpphandler.h"
#include "settings.h"
#include "log.h"
#include "bunnymanager.h"

OpenJabNab::OpenJabNab(int argc, char ** argv):QCoreApplication(argc, argv)
{
	connect(this, SIGNAL(aboutToQuit()), this, SLOT(OnQuit()));

	// Create PluginManager
	pluginManager = new PluginManager();
	
	// Create ApiManager
	apiManager = new ApiManager(pluginManager);
	
	// Create Listeners
	httpListener = new QTcpServer(this);
	httpListener->listen(QHostAddress::LocalHost, GlobalSettings::GetInt("OpenJabNabServers/ListeningHttpPort", 8080));
	connect(httpListener, SIGNAL(newConnection()), this, SLOT(NewHTTPConnection()));

	xmppListener = new QTcpServer(this);
	xmppListener->listen(QHostAddress::Any, GlobalSettings::GetInt("OpenJabNabServers/XmppPort", 5222));
	connect(xmppListener, SIGNAL(newConnection()), this, SLOT(NewXMPPConnection()));
}

void OpenJabNab::OnQuit()
{
	delete this;
}

OpenJabNab::~OpenJabNab()
{
	Log::Info("OpenJabNab closing...");
	delete pluginManager;
	delete apiManager;
	BunnyManager::Close();
}

void OpenJabNab::NewHTTPConnection()
{
	new HttpHandler(httpListener->nextPendingConnection(), pluginManager, apiManager);
}

void OpenJabNab::NewXMPPConnection()
{
	new XmppHandler(xmppListener->nextPendingConnection(), pluginManager);
}
