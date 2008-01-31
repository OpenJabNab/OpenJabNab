#include "xmpphandler.h"
#include "openjabnab.h"
#include "settings.h"
#include "log.h"

#include <QRegExp>

XmppHandler::XmppHandler(QTcpSocket * s, PluginManager * p)
{
	incomingXmppSocket = s;
	pluginManager = p;
	connect(incomingXmppSocket, SIGNAL(readyRead()), this, SLOT(handleBunnyXmppMessage()));
	connect(incomingXmppSocket, SIGNAL(disconnected()), this, SLOT(onDisconnect())); 
	
	outgoingXmppSocket.connectToHost(GlobalSettings::GetString("DefaultVioletServers/XmppServer"), 5222);
	connect(&outgoingXmppSocket, SIGNAL(readyRead()), this, SLOT(handleVioletXmppMessage()));
}

void XmppHandler::onDisconnect()
{
	delete this;
}

void XmppHandler::handleBunnyXmppMessage()
{
	QByteArray data = incomingXmppSocket->readAll();
	
	data.replace(GlobalSettings::GetString("OpenJabNabServers/XmppServer").toAscii(),GlobalSettings::GetString("DefaultVioletServers/XmppServer").toAscii());

	// Send info to all plugins
	pluginManager->XmppBunnyMessage(data);

	// Try to handle it
	// Check if the data contains <message></message>
	QRegExp rx("<message[^>]*>(.*)</message>");
	if (rx.indexIn(data) == -1)
	{
		// Just some signaling informations, forward directly
		outgoingXmppSocket.write(data);
		return;
	}
	
	QString message = rx.cap(1);
	
	// Parse message
	bool handled;
	if (message.startsWith("<button"))
	{
		// Single Click : <button xmlns="violet:nabaztag:button"><clic>1</clic></button>
		// Dble Click : <button xmlns="violet:nabaztag:button"><clic>2</clic></button>
		QRegExp rx("<clic>([0-9]+)</clic>");
		if (rx.indexIn(message) != -1)
		{
			int value = rx.cap(1).toInt();
			if (value == 1)
				handled = pluginManager->OnClick(PluginInterface::SingleClick);
			else if (value == 2)
				handled = pluginManager->OnClick(PluginInterface::DoubleClick);
			else
				Log::Warning("Unable to parse button/click message : " + data);
		}
		else
			Log::Warning("Unable to parse button message : " + data);
	}
	else if (message.startsWith("<ears"))
	{
		// <ears xmlns="violet:nabaztag:ears"><left>0</left><right>0</right></ears>
		QRegExp rx("<left>([0-9]+)</left><right>([0-9]+)</right>");
		if (rx.indexIn(message) != -1)
		{
			int left = rx.cap(1).toInt();
			int right = rx.cap(2).toInt();
			handled = pluginManager->OnEarsMove(left, right);
		}
		else
			Log::Warning("Unable to parse ears message : " + data);
	}
	if (!handled)
	{
		// Forward it to violet
		outgoingXmppSocket.write(data);
	}
}

void XmppHandler::handleVioletXmppMessage()
{
	QByteArray data = outgoingXmppSocket.readAll();
	
	data.replace(GlobalSettings::GetString("DefaultVioletServers/XmppServer").toAscii(),GlobalSettings::GetString("OpenJabNabServers/XmppServer").toAscii());
	
	pluginManager->XmppVioletMessage(data);
	
	incomingXmppSocket->write(data);
}
