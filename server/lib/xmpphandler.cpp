#include <QDateTime>
#include <QRegExp>
#include "bunny.h"
#include "bunnymanager.h"
#include "iq.h"
#include "log.h"
#include "messagepacket.h"
#include "netdump.h"
#include "openjabnab.h"
#include "settings.h"
#include "ttsmanager.h"
#include "xmpphandler.h"

unsigned short XmppHandler::msgNb = 0;

XmppHandler::XmppHandler(QTcpSocket * s, bool standAlone):pluginManager(PluginManager::Instance())
{
	incomingXmppSocket = s;
	bunny = 0;
	currentAuthStep = 0;

	isStandAlone = standAlone;

	// Bunny -> OpenJabNab socket
	connect(incomingXmppSocket, SIGNAL(disconnected()), this, SLOT(Disconnect()));
	incomingXmppSocket->setParent(this);

	// OpenJabNab -> Violet socket
	if(!isStandAlone)
	{
		outgoingXmppSocket = new QTcpSocket(this);
		outgoingXmppSocket->connectToHost(GlobalSettings::GetString("DefaultVioletServers/XmppServer"), 5222);
		connect(outgoingXmppSocket, SIGNAL(connected()), this, SLOT(VioletConnected()));
		connect(outgoingXmppSocket, SIGNAL(readyRead()), this, SLOT(HandleVioletXmppMessage()));
		connect(outgoingXmppSocket, SIGNAL(disconnected()), this, SLOT(Disconnect()));
	}
	else
	{
		// Stand alone mode, we can handle bunny messages directly
		connect(incomingXmppSocket, SIGNAL(readyRead()), this, SLOT(HandleBunnyXmppMessage()));
	}

	OjnXmppDomain = GlobalSettings::GetString("OpenJabNabServers/XmppServer").toAscii();
	VioletXmppDomain = GlobalSettings::GetString("DefaultVioletServers/XmppDomain").toAscii();
}

void XmppHandler::Disconnect()
{
	incomingXmppSocket->abort();
	if(!isStandAlone)
		outgoingXmppSocket->abort();
	if(bunny)
	{
		bunny->RemoveXmppHandler(this);
		bunny = 0;
	}

	delete incomingXmppSocket;
	delete outgoingXmppSocket;
	deleteLater();
}

void XmppHandler::VioletConnected()
{
	if(incomingXmppSocket->bytesAvailable())
		HandleBunnyXmppMessage();
	connect(incomingXmppSocket, SIGNAL(readyRead()), this, SLOT(HandleBunnyXmppMessage()));
}

void XmppHandler::HandleBunnyXmppMessage()
{
	QByteArray data = incomingXmppSocket->readAll();
	bool handled = false;
	if(data != " ") {
		if(bunny)
			NetworkDump::Log(QString("XMPP Bunny (%1)").arg(QString(bunny->GetID())), data);
		else
			NetworkDump::Log("XMPP Bunny", data);
	}
	// Replace OpenJabNab's domain if we are connected to Violet
	if(!isStandAlone)
		data.replace(OjnXmppDomain, VioletXmppDomain);

	// If we don't know which bunny is connected, try to authenticate it
	if (!bunny || !bunny->IsAuthenticated())
	{
		QByteArray ret;
		// Authentication error, disconnect
		if(pluginManager.GetAuthPlugin()->DoAuth(this, data, &bunny, ret) == false)
		{
			Disconnect();
			return;
		}
		// Answer to bunny if needed
		if(!ret.isNull())
		{
			WriteToBunnyAndLog(ret);
			return;
		}
	}

	// No bunny yet, forward unless we are in standAlone mode
	if(!bunny)
	{
		if(!isStandAlone)
			outgoingXmppSocket->write(data);
		else
			LogError(QString("Unable to handle xmpp message : %1").arg(QString(data)));
		return;
	}

	// Send raw xml info to all 'system' plugins and bunny's plugins
	bunny->XmppBunnyMessage(data);

	// Parse Bunny messages
	// Check if the data contains <message></message>
	QRegExp rx("<message[^>]*>(.*)</message>");
	if (rx.indexIn(data) != -1)
	{
		QString message = rx.cap(1);
		if (message.startsWith("<button"))
		{
			// Single Click : <button xmlns="violet:nabaztag:button"><clic>1</clic></button>
			// Double Click : <button xmlns="violet:nabaztag:button"><clic>2</clic></button>
			QRegExp rx("<clic>([0-9]+)</clic>");
			if (rx.indexIn(message) != -1)
			{
				int value = rx.cap(1).toInt();
				if (value == 1)
					handled = bunny->OnClick(PluginInterface::SingleClick);
				else if (value == 2)
					handled = bunny->OnClick(PluginInterface::DoubleClick);
				else
					LogWarning(QString("Unable to parse button/click message : %1").arg(QString(data)));
			}
			else
				LogWarning(QString("Unable to parse button message : %1").arg(QString(data)));
		}
		else if (message.startsWith("<ears"))
		{
			// <ears xmlns="violet:nabaztag:ears"><left>0</left><right>0</right></ears>
			QRegExp rx("<left>([0-9]+)</left><right>([0-9]+)</right>");
			if (rx.indexIn(message) != -1)
				handled = bunny->OnEarsMove(rx.cap(1).toInt(), rx.cap(2).toInt());
			else
				LogWarning(QString("Unable to parse ears message : %1").arg(QString(data)));
		}
		else
			LogWarning(QString("Unknown message from bunny : %1").arg(QString(data)));
	}
	else if (rx.setPattern("<iq.*/iq>"), rx.indexIn(data) != -1)
	{
		IQ iq(data);
		if(iq.IsValid())
		{
			if(rx.setPattern("<bind[^>]*><resource>([^<]*)</resource></bind>"), rx.indexIn(iq.Content()) != -1)
			{
				QByteArray resource = rx.cap(1).toAscii();
				bunny->SetXmppResource(resource);
				if(isStandAlone)
				{
					QByteArray from = bunny->GetID()+"@"+OjnXmppDomain+"/"+resource;
					WriteToBunnyAndLog(iq.Reply(IQ::Iq_Result, "%1 %4", "<bind xmlns='urn:ietf:params:xml:ns:xmpp-bind'><jid>"+from+"</jid></bind>"));
					handled = true;
				}
			}
			else if(iq.Content() == "<session xmlns='urn:ietf:params:xml:ns:xmpp-session'/>")
			{
				if(isStandAlone)
				{
					WriteToBunnyAndLog(iq.Reply(IQ::Iq_Result, "%4 %3 %2 %1", "<session xmlns='urn:ietf:params:xml:ns:xmpp-session'/>"));
					handled = true;
				}
			}
			else if(iq.Content() == "<query xmlns=\"violet:iq:sources\"><packet xmlns=\"violet:packet\" format=\"1.0\"/></query>")
			{
				if(isStandAlone)
				{
					QByteArray status = bunny->GetInitPacket();
					WriteToBunnyAndLog(iq.Reply(IQ::Iq_Result, "%2 %3 %1 %4", "<query xmlns='violet:iq:sources'><packet xmlns='violet:packet' format='1.0' ttl='604800'>"+(status.toBase64())+"</packet></query>"));
					handled = true;
				}
			}
			else if(rx.setPattern("<unbind[^>]*><resource>([^<]*)</resource></unbind>"), rx.indexIn(iq.Content()) != -1)
			{
				if(rx.cap(1) == "boot")
				{
					// Boot process finished
					bunny->Ready();
				}
				if(isStandAlone)
				{
					WriteToBunnyAndLog(iq.Reply(IQ::Iq_Result, "%1 %4", QByteArray()));
					handled = true;
				}
			}
			else
			{
				LogError(QString("Unknown IQ : %1").arg(QString(iq.Content())));
			}
		}
		else
		{
			LogError(QString("Invalid IQ : %1").arg(QString(data)));
		}
	}
	else if(rx.setPattern("<presence from='(.*)' id='(.*)'></presence>"), rx.indexIn(data) != -1)
	{
		if(isStandAlone)
		{
			QByteArray from = rx.cap(1).toAscii();
			QByteArray id = rx.cap(2).toAscii();
			WriteToBunnyAndLog("<presence from='"+from+"' to='"+from+"' id='"+id+"'/>");
			handled = true;
		}
	}
	else if(data.length() == 1 && isStandAlone)
	{
		// Bunny's ping packet, nothing to do
		handled = true;
	}

	// If the message wasn't handled, forward it to Violet unless we are in standAlone mode
	if (!handled)
	{
		if(!isStandAlone)
			outgoingXmppSocket->write(data);
		else
			LogError(QString("Unable to handle bunny xmpp message : %1").arg(QString(data)));
	}
}

void XmppHandler::HandleVioletXmppMessage()
{
	QByteArray data = outgoingXmppSocket->readAll();

	QList<QByteArray> list = XmlParse(data);
	foreach(QByteArray msg, list)
	{
		NetworkDump::Log("XMPP Violet", msg);

		if(msg.startsWith("</stream:stream>"))
		{
			// Disconnect as soon as possible
			QTimer::singleShot(0, this, SLOT(Disconnect()));
			return;
		}

		if(!bunny)
		{
			// Send info to all 'system' plugins only and forward
			pluginManager.XmppVioletMessage(bunny, msg);
			WriteToBunny(msg);
			continue;
		}
		else
			bunny->XmppVioletMessage(msg); 	// Send info to all 'system' plugins and bunny's plugins


		// Check if the data contains <packet></packet>
		QRegExp rx("<packet[^>]*format='([^']*)'[^>]*>(.*)</packet>");
		if (rx.indexIn(msg) != -1)
		{
			bool drop = false;
			// Try to decode it
			try
			{
				if (rx.cap(1) != "1.0")
					throw QString("Unknown packet format : %1").arg(QString(msg));

				try
				{
					QList<Packet*> list = Packet::Parse(QByteArray::fromBase64(rx.cap(2).toAscii()));
					QList<Packet*>::iterator it = list.begin();
					while(it != list.end())
					{
						NetworkDump::Log("XMPP Violet Packet", (*it)->GetPrintableData());
						if (bunny->XmppVioletPacketMessage(**it))
						{
							delete *it; // Packed used and dropped
							it = list.erase(it);
							drop = true; // List changed
						}
						else
							it++;
					}
					if(drop) // If list changed
					{
						if(list.count()) // If there is still one packet to send
						{
							QByteArray data = Packet::GetData(list);
							NetworkDump::Log("XMPP Violet PacketSent", data);
							msg.replace(rx.cap(2), data.toBase64());
							drop = false;
						}
						else
							drop = true; // Nothing to send, drop all
					}
					// Free Packets
					foreach(Packet * p, list)
						delete p;
				}
				catch (QString const& errorMsg)
				{
					LogWarning(errorMsg);
				}
			}
			catch (QString const& errorMsg)
			{
				LogWarning(errorMsg);
				// Can't handle it so forward it to the bunny
				WriteToBunny(msg);
			}
			if (!drop)
				WriteToBunny(msg);
		}
		else
		{
			// Just some signaling informations, forward directly
			WriteToBunny(msg);
		}
	}
}

void XmppHandler::WriteToBunny(QByteArray const& d)
{
	// Replace Violet's domain
	QByteArray data = d;
	data.replace(VioletXmppDomain, OjnXmppDomain);

	incomingXmppSocket->write(data);
	incomingXmppSocket->flush();
}

void XmppHandler::WriteToBunnyAndLog(QByteArray const& d)
{
	if(bunny)
		NetworkDump::Log(QString("XMPP To Bunny (%1)").arg(QString(bunny->GetID())), d);
	else
		NetworkDump::Log("XMPP To Bunny", d);
	WriteToBunny(d);
}


void XmppHandler::WriteDataToBunny(QByteArray const& b)
{
	if(bunny)
	{
		QByteArray msg;
		msg.append("<message from='net.openjabnab.platform@" + OjnXmppDomain + "/services' ");
		msg.append("to='" + bunny->GetID() + "@" + OjnXmppDomain + "/" + bunny->GetXmppResource() + "' ");
		msg.append("id='OJaNa-" + QByteArray::number(msgNb) + "'>");
		msg.append("<packet xmlns='violet:packet' format='1.0' ttl='604800'>");
		msg.append(b.toBase64());
		msg.append("</packet></message>");
		NetworkDump::Log(QString("XMPP To Bunny (%1)").arg(QString(bunny->GetID())), msg);
		WriteToBunny(msg);
		msgNb++;
	}
}

QList<QByteArray> XmppHandler::XmlParse(QByteArray const& data)
{
	QList<QByteArray> list;
	msgQueue += data.trimmed();

	QRegExp rx;

	while(!msgQueue.isEmpty())
	{
		if (rx.setPattern("^(<\\?xml[^>]*\\?><stream:stream[^>]*>)"), rx.indexIn(msgQueue) != -1)
		{
			list << rx.cap(1).toAscii();
			msgQueue.remove(0, rx.matchedLength());
		}
		else if (rx.setPattern("^(<[^>]*/>)"), rx.indexIn(msgQueue) != -1)
		{
			list << rx.cap(1).toAscii();
			msgQueue.remove(0, rx.matchedLength());
		}
		else if (rx.setPattern("^(</[^>]*>)"), rx.indexIn(msgQueue) != -1) // Special case for </stream:stream>
		{
			list << rx.cap(1).toAscii();
			msgQueue.remove(0, rx.matchedLength());
		}
		else
		{
			// Full xml message (<xxx>....</xxx>)
			// Find tag name
			rx.setPattern("^<([^ >]*)");
			if (rx.indexIn(msgQueue) == -1)
			{
				// Doesn't find the start tag... wait next message
				break;
			}
			QString tagName = rx.cap(1);
			// Search end tag
			rx.setPattern(QString("(.*</%1>)").arg(tagName));
			rx.setMinimal(true);
			if (rx.indexIn(msgQueue) == -1)
			{
				// Doesn't find the end tag... wait next message
				break;
			}
			list << rx.cap(1).toAscii();
			msgQueue.remove(0, rx.matchedLength());
		}
	}
	return list;
}
