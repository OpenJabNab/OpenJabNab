#include <QTcpSocket>
#include <QString>

#include "openjabnab.h"
#include "httphandler.h"
#include "xmpphandler.h"
#include "settings.h"
#include "log.h"

OpenJabNab::OpenJabNab(int argc, char ** argv):QCoreApplication(argc, argv)
{
	// Create PluginManager
	pluginManager = new PluginManager();

	// Create Listeners

	httpListener = new QTcpServer(this);
	httpListener->listen(QHostAddress::Any, GlobalSettings::GetInt("OpenJabNabServers/ListeningHttpPort", 8080));
	connect(httpListener, SIGNAL(newConnection()), this, SLOT(newHTTPConnection()));

	xmppListener = new QTcpServer(this);
	xmppListener->listen(QHostAddress::Any, GlobalSettings::GetInt("OpenJabNabServers/XmppPort", 5222));
	connect(xmppListener, SIGNAL(newConnection()), this, SLOT(newXMPPConnection()));
}

OpenJabNab::~OpenJabNab()
{
	Log::Info("OpenJabNab closing...");
	delete pluginManager;
}

void OpenJabNab::newHTTPConnection()
{
	new HttpHandler(httpListener->nextPendingConnection(), pluginManager);
}

void OpenJabNab::newXMPPConnection()
{
	new XmppHandler(xmppListener->nextPendingConnection(), pluginManager);
}
