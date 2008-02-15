#include "xmpphandler.h"
#include "openjabnab.h"
#include "settings.h"
#include "log.h"
#include "bunnymanager.h"

#include <QRegExp>
#include <QDateTime>

unsigned short XmppHandler::msgNb = 0;

XmppHandler::XmppHandler(QTcpSocket * s, PluginManager * p)
{
	incomingXmppSocket = s;
	pluginManager = p;
	bunny = 0;
	
	connect(incomingXmppSocket, SIGNAL(readyRead()), this, SLOT(HandleBunnyXmppMessage()));
	connect(incomingXmppSocket, SIGNAL(disconnected()), this, SLOT(OnDisconnect())); 
	
	outgoingXmppSocket.connectToHost(GlobalSettings::GetString("DefaultVioletServers/XmppServer"), 5222);
	connect(&outgoingXmppSocket, SIGNAL(readyRead()), this, SLOT(HandleVioletXmppMessage()));
}

void XmppHandler::OnDisconnect()
{
	delete this;
}

void XmppHandler::HandleBunnyXmppMessage()
{
	QByteArray data = incomingXmppSocket->readAll();
	
	data.replace(GlobalSettings::GetString("OpenJabNabServers/XmppServer").toAscii(),GlobalSettings::GetString("DefaultVioletServers/XmppServer").toAscii());

	// Send info to all plugins
	pluginManager->XmppBunnyMessage(data);
	
	QRegExp rx("<response[^>]*>(.*)</response>");
	if (rx.indexIn(data) != -1)
	{
		// Response message contains username, catch it to create Bunny
		QByteArray authString = QByteArray::fromBase64(rx.cap(1).toAscii());
		rx.setPattern("username=[\'\"]([^\'\"]*)[\'\"]");
		if (rx.indexIn(authString) != -1)
		{
			QByteArray bunnyID = rx.cap(1).toAscii();
			bunny = BunnyManager::GetBunny(bunnyID);
			bunny->SetXmppHandler(this);
			bunny->SetGlobalSetting("Last JabberConnection", QDateTime::currentDateTime());
		}
		else
			Log::Warning("Unable to parse response message : " + authString);
	}
	
	// Try to handle it
	// Check if the data contains <message></message>
	rx.setPattern("<message[^>]*>(.*)</message>");
	if (rx.indexIn(data) == -1)
	{
		// Just some signaling informations, forward directly
		outgoingXmppSocket.write(data);
		return;
	}
	
	QString message = rx.cap(1);
	
	// Parse message
	bool handled = false;
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

void XmppHandler::HandleVioletXmppMessage()
{
	QByteArray data = outgoingXmppSocket.readAll();
	
	data.replace(GlobalSettings::GetString("DefaultVioletServers/XmppServer").toAscii(),GlobalSettings::GetString("OpenJabNabServers/XmppServer").toAscii());

	QList<QByteArray> list = XmlParse(data);
	foreach(QByteArray msg, list)
	{
		// Send info to all plugins
		pluginManager->XmppVioletMessage(msg);

		// Check if the data contains <message></message>
		QRegExp rx("<message[^>]*>.*</message>");
		if (rx.indexIn(msg) != -1)
		{
			// Try to decode it
			try
			{
				rx.setPattern("<packet[^>]*format='([^']*)'[^>]*>(.*)</packet>");
				if (rx.indexIn(msg) == -1)
					throw "Unable to parse message : " + msg;

				if (rx.cap(1) != "1.0")
					throw "Unknown packet format : " + msg;

				try
				{
					Packet * p = Packet::Parse(QByteArray::fromBase64(rx.cap(2).toAscii()));
					bool drop = pluginManager->XmppVioletPacketMessage(*p);
					delete p;
					if(drop)
						continue;
				}
				catch (QByteArray const& errorMsg)
				{
					Log::Warning(errorMsg);
				}
				WriteToBunny(msg);
			}
			catch (QByteArray const& errorMsg)
			{
				Log::Warning(errorMsg);
				// Can't handle it so forward it to the bunny
				WriteToBunny(msg);
			}
		}
		else
		{
			// Just some signaling informations, forward directly
			WriteToBunny(msg);
		}
	}
}

void XmppHandler::WriteToBunny(QByteArray const& data)
{
	incomingXmppSocket->write(data);
	incomingXmppSocket->flush();
}

void XmppHandler::WritePacketToBunny(Packet const& p)
{
	QByteArray msg;
	msg.append("<message from='net.openjabnab.platform@" + GlobalSettings::GetString("OpenJabNabServers/XmppServer").toAscii() + "/services' ");
	msg.append("to='"  "------------"  "@" + GlobalSettings::GetString("OpenJabNabServers/XmppServer").toAscii() + "/idle' ");
	msg.append("id='OJaNa-" + QByteArray::number(msgNb) + "'>");
	msg.append("<packet xmlns='violet:packet' format='1.0' ttl='604800'>");
	msg.append(p.GetData().toBase64());
	msg.append("</packet></message>");
	WriteToBunny(msg);
	msgNb++;
}

QList<QByteArray> XmppHandler::XmlParse(QByteArray const& data)
{
	QList<QByteArray> list;
	QByteArray tmp = data.trimmed();

	QRegExp rxStream("^(<\\?xml[^>]*\\?><stream:stream[^>]*>)");
	QRegExp rxOne("^(<[^>]*/>)");

	while(!tmp.isEmpty())
	{
		if (rxStream.indexIn(tmp) != -1)
		{
			list << rxStream.cap(1).toAscii();
			tmp.remove(0, rxStream.matchedLength());
		}
		else if (rxOne.indexIn(tmp) != -1)
		{
			list << rxOne.cap(1).toAscii();
			tmp.remove(0, rxOne.matchedLength());
		}
		else
		{
			// Full xml message (<xxx>....</xxx>)
			// Find tag name
			QRegExp rxTag("^<([^ >]*)");
			if (rxTag.indexIn(tmp) == -1)
			{
				Log::Warning("Unable to parse : " + tmp);
				break;
			}
			QString tagName = rxTag.cap(1);
			// Search end tag
			rxTag.setPattern("(.*</" + tagName + ">)");
			rxTag.setMinimal(true);
			if (rxTag.indexIn(tmp) == -1)
			{
				Log::Warning("Unable to parse : " + tmp);
				break;
			}
			list << rxTag.cap(1).toAscii();
			tmp.remove(0, rxTag.matchedLength());
		}
	}
	return list;
}
