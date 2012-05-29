#include <QTcpSocket>
#include <QString>

#include "openjabnab.h"
#include "accountmanager.h"
#include "bunny.h"
#include "bunnymanager.h"
#include "ztamp.h"
#include "ztampmanager.h"
#include "httphandler.h"
#include "log.h"
#include "netdump.h"
#include "pluginmanager.h"
#include "settings.h"
#include "ttsmanager.h"
#include "xmpphandler.h"

OpenJabNab::OpenJabNab(int argc, char ** argv):QCoreApplication(argc, argv)
{
	GlobalSettings::Init();
	LogInfo("-- OpenJabNab Start --");
	TTSManager::Init();
	BunnyManager::Init();
	Bunny::Init();
	ZtampManager::Init();
	Ztamp::Init();
	AccountManager::Init();
	NetworkDump::Init();
	PluginManager::Init();
	BunnyManager::LoadBunnies();
	ZtampManager::LoadZtamps();

        int now = QDateTime::currentDateTime().toTime_t();
        int next = QDateTime(QDate::currentDate().addDays(1)).toTime_t();
        QTimer::singleShot(1000 * (next - now), this, SLOT(RotateLog()));

	if(GlobalSettings::Get("Config/HttpListener", true) == true)
	{
		// Create Listeners
		httpListener = new QTcpServer(this);
		httpListener->listen(QHostAddress::LocalHost, GlobalSettings::GetInt("OpenJabNabServers/ListeningHttpPort", 8080));
		connect(httpListener, SIGNAL(newConnection()), this, SLOT(NewHTTPConnection()));
	}
	else
		LogWarning("Warning : HTTP Listener is disabled !");

	if(GlobalSettings::Get("Config/XmppListener", true) == true)
	{
		int port = GlobalSettings::GetInt("OpenJabNabServers/ListeningXmppPort", 5222);
		LogInfo(QString("XMPP Port is: %1").arg(port));
		xmppListener = new QTcpServer(this);
		xmppListener->listen(QHostAddress::Any, port);
		connect(xmppListener, SIGNAL(newConnection()), this, SLOT(NewXMPPConnection()));
	}
	else
		LogWarning("Warning : XMPP Listener is disabled !");

	httpApi = GlobalSettings::Get("Config/HttpApi", true).toBool();
	httpVioletApi = GlobalSettings::Get("Config/HttpVioletApi", true).toBool();
	LogInfo(QString("Parsing of HTTP Api is ").append((httpApi == true)?"enabled":"disabled"));
}

void OpenJabNab::RotateLog()
{
	LogRotate();
	int now = QDateTime::currentDateTime().toTime_t();
	int next = QDateTime(QDate::currentDate().addDays(1)).toTime_t();
	QTimer::singleShot(1000 * (next - now), this, SLOT(RotateLog()));
}

void OpenJabNab::Close()
{
	emit Quit();
}

OpenJabNab::~OpenJabNab()
{
	if(xmppListener)
	{
		xmppListener->close();
	}
	if(httpListener)
	{
		httpListener->close();
	}
	NetworkDump::Close();
	ZtampManager::Close();
	BunnyManager::Close();
	TTSManager::Close();
	PluginManager::Close();
	AccountManager::Close();
	GlobalSettings::Close();
	LogInfo("-- OpenJabNab Close --");
}

void OpenJabNab::NewHTTPConnection()
{
	HttpHandler * h = new HttpHandler(httpListener->nextPendingConnection(), httpApi, httpVioletApi);
	connect(this, SIGNAL(Quit()), h, SLOT(Disconnect()));
}

void OpenJabNab::NewXMPPConnection()
{
	XmppHandler * x = new XmppHandler(xmppListener->nextPendingConnection());
	connect(this, SIGNAL(Quit()), x, SLOT(Disconnect()));
}
