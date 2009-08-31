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
	deleteLater();
}

void XmppHandler::VioletConnected()
{
	if(incomingXmppSocket->bytesAvailable())
		HandleBunnyXmppMessage();
	connect(incomingXmppSocket, SIGNAL(readyRead()), this, SLOT(HandleBunnyXmppMessage()));
}
#include <QCryptographicHash>
#define MD5(x) QCryptographicHash::hash(x, QCryptographicHash::Md5)
#define MD5_HEX(x) QCryptographicHash::hash(x, QCryptographicHash::Md5).toHex()
QByteArray ComputeResponse(QByteArray const& username, QByteArray const& password, QByteArray const& nonce, QByteArray const& cnonce, QByteArray const& nc, QByteArray const& digest_uri, QByteArray const& mode)
{
	QByteArray HA1 = MD5_HEX(MD5(username + "::" + password) + ":" + nonce + ":" + cnonce);
	QByteArray HA2 = MD5_HEX(mode + ":" + digest_uri);
	QByteArray response = MD5_HEX(HA1 + ":" + nonce + ":" + nc + ":" + cnonce + ":auth:" + HA2);
	return response;
}

QByteArray ComputeXor(QByteArray const& v1, QByteArray const& v2)
{
	QByteArray t1 = QByteArray::fromHex(v1);
	QByteArray t2 = QByteArray::fromHex(v2);
	for(int i = 0; i < t1.size(); i++)
	{
		t1[i] = (char)t1[i] ^ (char)t2[i];
	}
	return t1.toHex();
}

void XmppHandler::HandleBunnyXmppMessage()
{
	QByteArray data = incomingXmppSocket->readAll();
	bool handled = false;

	NetworkDump::Log("XMPP Bunny", data);
	
	// Replace OpenJabNab's domain if we are connected to Violet
	if(!isStandAlone)
		data.replace(OjnXmppDomain, VioletXmppDomain);

	// If we don't already know which bunny is connected, try to find a <response></response> message or, doing an authentication if we are stand alone
	if (!bunny || !bunny->IsAuthenticated())
	{
		if(isStandAlone)
		{
			switch(currentAuthStep)
			{
				case 0:
					// We should receive <?xml version='1.0' encoding='UTF-8'?><stream:stream to='ojn.soete.org' xmlns='jabber:client' xmlns:stream='http://etherx.jabber.org/streams' version='1.0'>"
					if(data.startsWith("<?xml version='1.0' encoding='UTF-8'?>"))
					{
						// Send an auth Request
						WriteToBunnyAndLog("<?xml version='1.0'?><stream:stream xmlns='jabber:client' xmlns:stream='http://etherx.jabber.org/streams' id='2173750751' from='xmpp.nabaztag.com' version='1.0' xml:lang='en'>");
						WriteToBunnyAndLog("<stream:features><mechanisms xmlns='urn:ietf:params:xml:ns:xmpp-sasl'><mechanism>DIGEST-MD5</mechanism><mechanism>PLAIN</mechanism></mechanisms><register xmlns='http://violet.net/features/violet-register'/></stream:features>");
						currentAuthStep = 1;
						return;
					}
					Log::Error("Bad Auth Step 0, disconnect");
					Disconnect();
					return;
					
				case 1:
					// We should receive <auth xmlns='urn:ietf:params:xml:ns:xmpp-sasl' mechanism='DIGEST-MD5'/>
					if(data.startsWith("<auth xmlns='urn:ietf:params:xml:ns:xmpp-sasl' mechanism='DIGEST-MD5'/>"))
					{
						// Send a challenge
						// <challenge xmlns='urn:ietf:params:xml:ns:xmpp-sasl'>...</challenge>
						// <challenge xmlns='urn:ietf:params:xml:ns:xmpp-sasl'>nonce="random_number",qop="auth",charset=utf-8,algorithm=md5-sess</challenge>
						QByteArray nonce = QByteArray::number((unsigned int)qrand());
						QByteArray challenge = "nonce=\"" + nonce + "\",qop=\"auth\",charset=utf-8,algorithm=md5-sess";
						WriteToBunnyAndLog("<challenge xmlns='urn:ietf:params:xml:ns:xmpp-sasl'>" + challenge.toBase64() + "</challenge>");
						currentAuthStep = 2;
						return;
					}
					if (data.startsWith("<iq type='get' id='1'><query xmlns='violet:iq:register'/></iq>")) // Bunny request a register
					{
						// Send the request
						WriteToBunnyAndLog("<iq from='" + OjnXmppDomain + "' id='1' type='result'><query xmlns='violet:iq:register'><instructions>Choose a username and password to register with this server</instructions><username/><password/></query></iq>");
						currentAuthStep = 100;
						return;
					}
					Log::Error("Bad Auth Step 1, disconnect");
					Disconnect();
					return;
					
				case 2:
					{
						// We should receive <response xmlns='urn:ietf:params:xml:ns:xmpp-sasl'>...</response>
						QRegExp rx("<response[^>]*>(.*)</response>");
						if (rx.indexIn(data) != -1)
						{
							QByteArray authString = QByteArray::fromBase64(rx.cap(1).toAscii()).replace((char)0, "");
							// authString is like : username="",nonce="",cnonce="",nc=,qop=auth,digest-uri="",response=,charset=utf-8
							// Parse values
							rx.setPattern("username=\"([^\"]*)\",nonce=\"([^\"]*)\",cnonce=\"([^\"]*)\",nc=([^,]*),qop=auth,digest-uri=\"([^\"]*)\",response=([^,]*),charset=utf-8");
							if(rx.indexIn(authString) != -1)
							{
								QByteArray const username = rx.cap(1).toAscii();
								bunny = BunnyManager::GetBunny(username);
								QByteArray const password = bunny->GetBunnyPassword();
								QByteArray const nonce = rx.cap(2).toAscii();
								QByteArray const cnonce = rx.cap(3).toAscii().append((char)0); // cnonce have a dummy \0 at his end :(
								QByteArray const nc = rx.cap(4).toAscii();
								QByteArray const digest_uri = rx.cap(5).toAscii();
								QByteArray const bunnyResponse = rx.cap(6).toAscii();
								if(bunnyResponse == ComputeResponse(username, password, nonce, cnonce, nc, digest_uri, "AUTHENTICATE"))
								{
									// Send challenge back
									// <challenge xmlns='urn:ietf:params:xml:ns:xmpp-sasl'>...</challenge>
									// rspauth=...
									QByteArray const rspAuth = "rspauth=" + ComputeResponse(username, password, nonce, cnonce, nc, digest_uri, "");
									WriteToBunnyAndLog("<challenge xmlns='urn:ietf:params:xml:ns:xmpp-sasl'>" + rspAuth.toBase64() + "</challenge>");
									currentAuthStep = 3;
									return;
								}
								else
								{
									// Bad password, send failure and restart auth
									WriteToBunnyAndLog("<failure xmlns='urn:ietf:params:xml:ns:xmpp-sasl'><not-authorized/></failure>");
									currentAuthStep = 0;
									return;
								}
							}
						}
						Log::Error("Bad Auth Step 2, disconnect");
						Disconnect();
						return;
					}
					
				case 3:
					// We should receive <response xmlns='urn:ietf:params:xml:ns:xmpp-sasl'/>
					if(data.startsWith("<response xmlns='urn:ietf:params:xml:ns:xmpp-sasl'/>"))
					{
						// Send success
						WriteToBunnyAndLog("<success xmlns='urn:ietf:params:xml:ns:xmpp-sasl'/>");
						currentAuthStep = 4;
						return;
					}
					Log::Error("Bad Auth Step 3, disconnect");
					Disconnect();
					return;
					
				case 4:
					// We should receive <?xml version='1.0' encoding='UTF-8'?>
					if(data.startsWith("<?xml version='1.0' encoding='UTF-8'?>"))
					{
						// Send success
						WriteToBunnyAndLog("<?xml version='1.0'?><stream:stream xmlns='jabber:client' xmlns:stream='http://etherx.jabber.org/streams' id='1331400675' from='xmpp.nabaztag.com' version='1.0' xml:lang='en'>");
						WriteToBunnyAndLog("<stream:features><bind xmlns='urn:ietf:params:xml:ns:xmpp-bind'><required/></bind><unbind xmlns='urn:ietf:params:xml:ns:xmpp-bind'/><session xmlns='urn:ietf:params:xml:ns:xmpp-session'/></stream:features>");
						currentAuthStep = 5;
						return;
					}
					Log::Error("Bad Auth Step 4, disconnect");
					Disconnect();
					return;

				case 5:
					{
						// We should receive <iq from="<mac_address>@<domain>" to="<domain>" type='set' id='1'>...
						QRegExp rx("<iq from=\"([^@]*)@"+OjnXmppDomain+"/\" to=\""+OjnXmppDomain+"\" type='set' id='1'><bind xmlns='urn:ietf:params:xml:ns:xmpp-bind'><resource>boot</resource></bind></iq>");
	                                        if (rx.indexIn(data) != -1)
	                                        {
							QByteArray const username = rx.cap(1).toAscii();
	
							WriteToBunnyAndLog("<iq id='1' type='result'><bind xmlns='urn:ietf:params:xml:ns:xmpp-bind'><jid>"+username+"@"+VioletXmppDomain+"/boot</jid></bind></iq>");
							currentAuthStep = 6;
							return;
						}
						Log::Error("Bad Auth Step 5, disconnect");
						Disconnect();
						return;
					}
				case 6:
					{
						// We should receive <iq from="<mac_address>@<domain>/boot" to="<domain>" type='set' id='2'>...
						QRegExp rx("<iq from='([^@]*)@"+OjnXmppDomain+"/boot' to='"+OjnXmppDomain+"' type='set' id='2'><session xmlns='urn:ietf:params:xml:ns:xmpp-session'/></iq>");
	                                        if (rx.indexIn(data) != -1)
	                                        {
							QByteArray const username = rx.cap(1).toAscii();
	
							WriteToBunnyAndLog("<iq type='result' to='"+username+"@"+OjnXmppDomain+"/boot' from='"+OjnXmppDomain+"' id='2'><session xmlns='urn:ietf:params:xml:ns:xmpp-session'/></iq>");
							currentAuthStep = 7;
							return;
						}
						Log::Error("Bad Auth Step 6, disconnect");
						Disconnect();
						return;
					}
				case 7:
					{
						// We should receive <iq from="<mac_address>@<domain>/boot" to="net.violet.platform@<domain>/sources" type='set' id='3'>...
						QRegExp rx("<iq from='([^@]*)@"+OjnXmppDomain+"/boot' to='net.violet.platform@"+OjnXmppDomain+"/sources' type='get' id='3'><query xmlns=\"violet:iq:sources\"><packet xmlns=\"violet:packet\" format=\"1.0\"/></query></iq>");
	                                        if (rx.indexIn(data) != -1)
	                                        {
							QByteArray const username = rx.cap(1).toAscii();
							bunny = BunnyManager::GetBunny(username);
	
							WriteToBunnyAndLog("<iq from='net.violet.platform@"+OjnXmppDomain+"/sources' to='"+username+"@"+OjnXmppDomain+"/boot' id='3' type='result'><query xmlns='violet:iq:sources'><packet xmlns='violet:packet' format='1.0' ttl='604800'>fwQAAAx////+BBAFAA4oCAALAAABAP8=</packet></query></iq>");
							currentAuthStep = 8;
							return;
						}
						Log::Error("Bad Auth Step 7, disconnect");
						Disconnect();
						return;
					}
				case 8:
					{
						// We should receive <iq from="<mac_address>@<domain>/boot" to="net.violet.platform" type='set' id='4'>...
						QRegExp rx("<iq from=\"([^@]*)@"+OjnXmppDomain+"/boot\" to=\""+OjnXmppDomain+"\" type='set' id='4'><bind xmlns='urn:ietf:params:xml:ns:xmpp-bind'><resource>idle</resource></bind></iq>");
	                                        if (rx.indexIn(data) != -1)
	                                        {
							QByteArray const username = rx.cap(1).toAscii();
							bunny = BunnyManager::GetBunny(username);
	
							WriteToBunnyAndLog("<iq id='4' type='result'><bind xmlns='urn:ietf:params:xml:ns:xmpp-bind'><jid>"+username+"@"+OjnXmppDomain+"/idle</jid></bind></iq>");
							currentAuthStep = 9;
							return;
						}
						Log::Error("Bad Auth Step 8, disconnect");
						Disconnect();
						return;
					}
				case 9:
					{
						// We should receive <iq from="<mac_address>@<domain>/idle" to="<domain>" type='set' id='5'>...
						QRegExp rx("<iq from='([^@]*)@"+OjnXmppDomain+"/idle' to='"+OjnXmppDomain+"' type='set' id='5'><session xmlns='urn:ietf:params:xml:ns:xmpp-session'/></iq>");
	                                        if (rx.indexIn(data) != -1)
	                                        {
							QByteArray const username = rx.cap(1).toAscii();
							bunny = BunnyManager::GetBunny(username);
	
							WriteToBunnyAndLog("<iq type='result' to='"+username+"+@"+OjnXmppDomain+"/idle' from='"+OjnXmppDomain+"' id='5'><session xmlns='urn:ietf:params:xml:ns:xmpp-session'/></iq>");
							currentAuthStep = 10;
							return;
						}
						Log::Error("Bad Auth Step 9, disconnect");
						Disconnect();
						return;
					}
				case 10:
					{
						// We should receive <presence from="<mac_address>@<domain>/idle" id='5'></presence>
						QRegExp rx("<presence from='([^@]*)@"+OjnXmppDomain+"/idle' id='6'></presence>");
	                                        if (rx.indexIn(data) != -1)
	                                        {
							QByteArray const username = rx.cap(1).toAscii();
							bunny = BunnyManager::GetBunny(username);
	
							WriteToBunnyAndLog("<presence from='"+username+"@"+OjnXmppDomain+"/idle' to='"+username+"@"+OjnXmppDomain+"/idle' id='6'/>");
							currentAuthStep = 11;
							return;
						}
						Log::Error("Bad Auth Step 10, disconnect");
						Disconnect();
						return;
					}
				case 11:
					{
						// We should receive <iq from="<mac_address>@<domain>/boot" to="<domain>" type='set' id='7'>...
						QRegExp rx("<iq from='([^@]*)@"+OjnXmppDomain+"/boot' to='"+OjnXmppDomain+"' type='set' id='7'><unbind xmlns='urn:ietf:params:xml:ns:xmpp-bind'><resource>boot</resource></unbind></iq>");
	                                        if (rx.indexIn(data) != -1)
	                                        {
							QByteArray const username = rx.cap(1).toAscii();
							bunny = BunnyManager::GetBunny(username);
	
							WriteToBunnyAndLog("<iq id='7' type='result'/>");
							bunny->SetXmppHandler(this);
							bunny->SetGlobalSetting("Last JabberConnection", QDateTime::currentDateTime());
							currentAuthStep = 12;
							if(GlobalSettings::Get("Config/StandAloneWelcome", false) == true)
							{
								QDir httpLocalFolder(GlobalSettings::GetString("Config/RealHttpRoot"));
								TTSManager::CreateNewSound("Bienvenue sur open Jab Nab", "claire", httpLocalFolder.absoluteFilePath("welcome_ojn.mp3"));
								QByteArray message = "MU broadcast/"+GlobalSettings::GetString("Config/HttpRoot").toAscii()+"/welcome_ojn.mp3 \nPL 3\nMW\n";
								bunny->SendPacket(MessagePacket(message));
							}
							return;
						}
						Log::Error("Bad Auth Step 11, disconnect");
						Disconnect();
						return;
					}
				case 100: // Register Bunny
					{
						// We should receive <iq to='xmpp.nabaztag.com' type='set' id='2'><query xmlns="violet:iq:register"><username>0019db01dbd7</username><password>208e6d83bfb2</password></query></iq>
						IQ iqAuth(data);
						if(iqAuth.IsValid() && iqAuth.Type() == IQ::Iq_Set)
						{
							QByteArray content = iqAuth.Content();
							QRegExp rx("<query xmlns=\"violet:iq:register\"><username>([0-9a-f]*)</username><password>([0-9a-f]*)</password></query>");
							if(rx.indexIn(content) != -1)
							{
								QByteArray user = rx.cap(1).toAscii();
								QByteArray password = rx.cap(2).toAscii();
								bunny = BunnyManager::GetBunny(user);
								if(bunny->SetBunnyPassword(ComputeXor(user,password)))
								{
									WriteToBunnyAndLog(iqAuth.Reply(IQ::Iq_Result, content));
									currentAuthStep = 1;
									return;
								}
								Log::Error("Password already set for bunny " + user);
								Disconnect();
								return;
							}
						}
						Log::Error("Bad Register, disconnect");
						Disconnect();
						return;
					}
					
				default:
					Log::Error("Unknown Auth Step, disconnect");
					Disconnect();
					return;
			}
		}
		else
		{
			QRegExp rx("<response[^>]*>(.*)</response>");
			if (rx.indexIn(data) != -1)
			{
				// Response message contains username, catch it to create the Bunny
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
					Log::Warning(QString("Unable to parse response message : %1").arg(QString(authString)));

				// Login hack	
				authString = authString.replace((char)0, "");
				rx.setPattern("username=\"([^\"]*)\",nonce=\"([^\"]*)\",cnonce=\"([^\"]*)\",nc=([^,]*),qop=auth,digest-uri=\"([^\"]*)\",response=([^,]*),charset=utf-8");
				if(rx.indexIn(authString) != -1)
				{
					QByteArray const username = rx.cap(1).toAscii();
					bunny = BunnyManager::GetBunny(username);
					QByteArray const password = GlobalSettings::GetString("Config/VioletPassword").toAscii();
					QByteArray const nonce = rx.cap(2).toAscii();
					QByteArray const cnonce = rx.cap(3).toAscii().append((char)0); // cnonce have a dummy \0 at his end :(
					QByteArray const nc = rx.cap(4).toAscii();
					QByteArray const digest_uri = rx.cap(5).toAscii();
					QByteArray const bunnyResponse = rx.cap(6).toAscii();
					QByteArray newAuthString = ComputeResponse(username, password, nonce, cnonce, nc, digest_uri, "AUTHENTICATE");
					newAuthString = "username=\""+username+"\",nonce=\""+nonce+"\",cnonce=\""+cnonce+"\",nc="+nc+",qop=auth,digest-uri=\""+digest_uri+"\",response="+newAuthString+",charset=utf-8";
					Log::Info(authString+"\n\n"+newAuthString.replace((char)0, "")+"\n\n");
					newAuthString = "<response xmlns='urn:ietf:params:xml:ns:xmpp-sasl'>"+newAuthString.toBase64()+"</response>";
					Log::Info(data+" "+newAuthString);
					data=newAuthString;
					Log::Info(data+" "+newAuthString);
				}
			}
		}
	}

	// No bunny yet, forward unless we are in standAlone mode
	if(!bunny)
	{
		// Send info to all 'system' plugins only and forward to violet unless we are in standAlone mode
		pluginManager.XmppBunnyMessage(bunny, data);
		if(isStandAlone)
			Log::Error(QString("Unable to handle xmpp message : %1").arg(QString(data)));
		else
			outgoingXmppSocket->write(data);
		return;
	}

	// Send info to all 'system' plugins and bunny's plugins
	bunny->XmppBunnyMessage(data);

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
					Log::Warning(QString("Unable to parse button/click message : %1").arg(QString(data)));
			}
			else
				Log::Warning(QString("Unable to parse button message : %1").arg(QString(data)));
		}
		else if (message.startsWith("<ears"))
		{
			// <ears xmlns="violet:nabaztag:ears"><left>0</left><right>0</right></ears>
			QRegExp rx("<left>([0-9]+)</left><right>([0-9]+)</right>");
			if (rx.indexIn(message) != -1)
				handled = bunny->OnEarsMove(rx.cap(1).toInt(), rx.cap(2).toInt());
			else
				Log::Warning(QString("Unable to parse ears message : %1").arg(QString(data)));
		}
		else
			Log::Warning(QString("Unknown message from bunny : %1").arg(QString(data)));
	}
	else if (rx.setPattern("<bind[^>]*><resource>([^<]*)</resource></bind>"), rx.indexIn(data) != -1)
	{
		//<bind xmlns='urn:ietf:params:xml:ns:xmpp-bind'><resource>idle</resource></bind>
		bunny->SetXmppResource(rx.cap(1).toAscii());
	}
	if(isStandAlone && data.length() == 1)
	{
		// Bunny is idle
		handled = true;
	}

	// If the message wasn't handled, forward it to Violet unless we are in standAlone mode
	if (!handled)
	{
		if(isStandAlone)
			Log::Error(QString("Unable to handle bunny xmpp message : %1").arg(QString(data)));
		else
			outgoingXmppSocket->write(data);
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
					Log::Warning(errorMsg);
				}
			}
			catch (QString const& errorMsg)
			{
				Log::Warning(errorMsg);
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
		NetworkDump::Log("XMPP To Bunny", msg);
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
