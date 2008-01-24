#include "xmpphandler.h"
#include "openjabnab.h"
#include "settings.h"

#ifdef DEBUG
	#define LOG(x) x
#else
	#define LOG(x)
#endif

XmppHandler::XmppHandler(QTcpSocket * s)
{
	incomingXmppSocket = s;
	connect(incomingXmppSocket, SIGNAL(readyRead()), this, SLOT(handleBunnyXmppMessage()));
	
	outgoingXmppSocket = new QTcpSocket();
	outgoingXmppSocket->connectToHost(GlobalSettings::GetString("DefaultVioletServers/XmppServer"), 5222);
	connect(outgoingXmppSocket, SIGNAL(readyRead()), this, SLOT(handleVioletXmppMessage()));
}

XmppHandler::~XmppHandler()
{
}

void XmppHandler::handleBunnyXmppMessage()
{
	QByteArray data = incomingXmppSocket->readAll();
	
	data.replace(GlobalSettings::GetString("OpenJabNabServers/XmppServer").toAscii(),GlobalSettings::GetString("DefaultVioletServers/XmppServer").toAscii());
	
	foreach(PluginInterface * plugin, OpenJabNab::GetPlugins())
	{
		plugin->XmppBunnyMessage(data);
	}
	outgoingXmppSocket->write(data);
}

void XmppHandler::handleVioletXmppMessage()
{
	QByteArray data = outgoingXmppSocket->readAll();
	
	data.replace(GlobalSettings::GetString("DefaultVioletServers/XmppServer").toAscii(),GlobalSettings::GetString("OpenJabNabServers/XmppServer").toAscii());
	
	foreach(PluginInterface * plugin, OpenJabNab::GetPlugins())
	{
		plugin->XmppVioletMessage(data);
	}
	incomingXmppSocket->write(data);
}
