#include <QTcpSocket>
#include <QByteArray>
#include <QDir>
#include <QPluginLoader>
#include <QString>
#include <QLibrary>

#include "openjabnab.h"
#include "httphandler.h"
#include "xmpphandler.h"
#include "settings.h"
#include "log.h"

OpenJabNab::OpenJabNab(int argc, char ** argv):QCoreApplication(argc, argv)
{
	// Load all plugins
	QDir pluginsDir = QCoreApplication::applicationDirPath();
	pluginsDir.cd("plugins");

	Log::Info(QString("Finding plugins in : %1").arg(pluginsDir.path()));
	
	foreach (QString fileName, pluginsDir.entryList(QDir::Files)) 
	{
		QString file = pluginsDir.absoluteFilePath(fileName);
		if (!QLibrary::isLibrary(file))
			continue;

		QString status = QString(" - %1 : ").arg(fileName);
		
		QPluginLoader loader(file);
		QObject * p = loader.instance();
		PluginInterface * plugin = qobject_cast<PluginInterface *>(p);
		if (plugin)
		{
			listOfPlugins.append(plugin);
			status.append(" OK");
		}
		else
		{
			status.append("Failed, ").append(loader.errorString()); 
		}
		Log::Info(status);
	}

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
	foreach(PluginInterface * plugin, listOfPlugins)
	{
		delete plugin;
	}
}

void OpenJabNab::newHTTPConnection()
{
	new HttpHandler(httpListener->nextPendingConnection());
}

void OpenJabNab::newXMPPConnection()
{
	new XmppHandler(xmppListener->nextPendingConnection());
}

QVector<PluginInterface *> OpenJabNab::listOfPlugins;
