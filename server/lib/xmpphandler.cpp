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

XmppHandler::XmppHandler(QTcpSocket * s):pluginManager(PluginManager::Instance())
{
	incomingXmppSocket = s;
	bunny = 0;
	currentAuthStep = 0;

	// Bunny -> OpenJabNab socket
	incomingXmppSocket->setParent(this);
	connect(incomingXmppSocket, SIGNAL(disconnected()), this, SLOT(Disconnect()));
	connect(incomingXmppSocket, SIGNAL(readyRead()), this, SLOT(HandleBunnyXmppMessage()));

	OjnXmppDomain = GlobalSettings::GetString("OpenJabNabServers/XmppServer").toAscii();
}

void XmppHandler::Disconnect()
{
	incomingXmppSocket->abort();
	if(bunny)
	{
		bunny->RemoveXmppHandler(this);
		bunny = 0;
	}

	deleteLater();
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

	// No bunny yet
	if(!bunny)
	{
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

				QByteArray from = bunny->GetID()+"@"+OjnXmppDomain+"/"+resource;
				WriteToBunnyAndLog(iq.Reply(IQ::Iq_Result, "%1 %4", "<bind xmlns='urn:ietf:params:xml:ns:xmpp-bind'><jid>"+from+"</jid></bind>"));
				handled = true;
			}
			else if(iq.Content() == "<session xmlns='urn:ietf:params:xml:ns:xmpp-session'/>")
			{
				WriteToBunnyAndLog(iq.Reply(IQ::Iq_Result, "%4 %3 %2 %1", "<session xmlns='urn:ietf:params:xml:ns:xmpp-session'/>"));
				handled = true;
			}
			else if(iq.Content() == "<query xmlns=\"violet:iq:sources\"><packet xmlns=\"violet:packet\" format=\"1.0\"/></query>")
			{
				QByteArray status = bunny->GetInitPacket();
				WriteToBunnyAndLog(iq.Reply(IQ::Iq_Result, "%2 %3 %1 %4", "<query xmlns='violet:iq:sources'><packet xmlns='violet:packet' format='1.0' ttl='604800'>"+(status.toBase64())+"</packet></query>"));
				handled = true;
			}
			else if(rx.setPattern("<unbind[^>]*><resource>([^<]*)</resource></unbind>"), rx.indexIn(iq.Content()) != -1)
			{
				if(rx.cap(1) == "boot")
				{
					// Boot process finished
					bunny->Ready();
				}
				WriteToBunnyAndLog(iq.Reply(IQ::Iq_Result, "%1 %4", QByteArray()));
				handled = true;
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
		QByteArray from = rx.cap(1).toAscii();
		QByteArray id = rx.cap(2).toAscii();
		WriteToBunnyAndLog("<presence from='"+from+"' to='"+from+"' id='"+id+"'/>");
		handled = true;
	}
	else if(data.length() == 1)
	{
		// Bunny's ping packet, nothing to do
		handled = true;
	}

	// If the message wasn't handled
	if (!handled)
	{
		LogError(QString("Unable to handle bunny xmpp message : %1").arg(QString(data)));
	}
}


void XmppHandler::WriteToBunny(QByteArray const& d)
{
	incomingXmppSocket->write(d);
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
