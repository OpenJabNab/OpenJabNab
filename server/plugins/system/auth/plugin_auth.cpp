#include <QDateTime>
#include <QStringList>
#include "plugin_auth.h"
#include "account.h"
#include "bunny.h"
#include "bunnymanager.h"
#include "iq.h"
#include "log.h"
#include "settings.h"
#include "xmpphandler.h"

Q_EXPORT_PLUGIN2(plugin_auth, PluginAuth)

PluginAuth::PluginAuth():PluginAuthInterface("auth", "Manage Authentication process")
{
}

// Helpers
#include <QCryptographicHash>
#define MD5(x) QCryptographicHash::hash(x, QCryptographicHash::Md5)
#define MD5_HEX(x) QCryptographicHash::hash(x, QCryptographicHash::Md5).toHex()
static QByteArray ComputeResponse(QByteArray const& username, QByteArray const& password, QByteArray const& nonce, QByteArray const& cnonce, QByteArray const& nc, QByteArray const& digest_uri, QByteArray const& mode)
{
	QByteArray HA1 = MD5_HEX(MD5(username + "::" + password) + ":" + nonce + ":" + cnonce);
	QByteArray HA2 = MD5_HEX(mode + ":" + digest_uri);
	QByteArray response = MD5_HEX(HA1 + ":" + nonce + ":" + nc + ":" + cnonce + ":auth:" + HA2);
	return response;
}

static QByteArray ComputeXor(QByteArray const& v1, QByteArray const& v2)
{
	QByteArray t1 = QByteArray::fromHex(v1);
	QByteArray t2 = QByteArray::fromHex(v2);
	for(int i = 0; i < t1.size(); i++)
	{
		t1[i] = (char)t1[i] ^ (char)t2[i];
	}
	return t1.toHex();
}

bool PluginAuth::DoAuth(XmppHandler * xmpp, QByteArray const& data, Bunny ** pBunny, QByteArray & answer)
{
	switch(xmpp->currentAuthStep)
	{
		case 0:
			// We should receive <?xml version='1.0' encoding='UTF-8'?><stream:stream to='ojn.soete.org' xmlns='jabber:client' xmlns:stream='http://etherx.jabber.org/streams' version='1.0'>"
			if(data.startsWith("<?xml version='1.0' encoding='UTF-8'?>"))
			{
				// Send an auth Request
				answer.append("<?xml version='1.0'?><stream:stream xmlns='jabber:client' xmlns:stream='http://etherx.jabber.org/streams' id='2173750751' from='"+ xmpp->GetXmppDomain() + "' version='1.0' xml:lang='en'>");
				answer.append("<stream:features><mechanisms xmlns='urn:ietf:params:xml:ns:xmpp-sasl'><mechanism>DIGEST-MD5</mechanism><mechanism>PLAIN</mechanism></mechanisms><register xmlns='http://violet.net/features/violet-register'/></stream:features>");
				xmpp->currentAuthStep = 1;
				return true;
			}
			LogError("Bad Auth Step 0, disconnect");
			return false;

		case 1:
			{
				// Bunny request a register <iq type='get' id='1'><query xmlns='violet:iq:register'/></iq>
				IQ iq(data);
				if(iq.IsValid() && iq.Type() == IQ::Iq_Get && iq.Content() == "<query xmlns='violet:iq:register'/>")
				{
					// Send the request
					answer = iq.Reply(IQ::Iq_Result, "from='" + xmpp->GetXmppDomain() + "' %1 %4", "<query xmlns='violet:iq:register'><instructions>Choose a username and password to register with this server</instructions><username/><password/></query>");
					xmpp->currentAuthStep = 100;
					return true;
				}
				// Bunny request an auth <auth xmlns='urn:ietf:params:xml:ns:xmpp-sasl' mechanism='DIGEST-MD5'/>
				if(data.startsWith("<auth xmlns='urn:ietf:params:xml:ns:xmpp-sasl' mechanism='DIGEST-MD5'/>"))
				{
					// Send a challenge
					// <challenge xmlns='urn:ietf:params:xml:ns:xmpp-sasl'>...</challenge>
					// <challenge xmlns='urn:ietf:params:xml:ns:xmpp-sasl'>nonce="random_number",qop="auth",charset=utf-8,algorithm=md5-sess</challenge>
					QByteArray nonce = QByteArray::number((unsigned int)qrand());
					QByteArray challenge = "nonce=\"" + nonce + "\",qop=\"auth\",charset=utf-8,algorithm=md5-sess";
					answer.append("<challenge xmlns='urn:ietf:params:xml:ns:xmpp-sasl'>" + challenge.toBase64() + "</challenge>");
					xmpp->currentAuthStep = 2;
					return true;
				}
				LogError("Bad Auth Step 1, disconnect");
				return false;
			}

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
						Bunny * bunny = BunnyManager::GetBunny(username);

						// Check if we want to bypass auth
						if(GlobalSettings::Get("Config/StandAloneAuthBypass", false) == true)
						{
							// Send success
							LogInfo("Sending success instead of password verification");
							answer.append("<success xmlns='urn:ietf:params:xml:ns:xmpp-sasl'/>");

							bunny->Authenticating();
							*pBunny = bunny; // Auth OK, set current bunny

							xmpp->currentAuthStep = 4;
							return true;
						}


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
							answer.append("<challenge xmlns='urn:ietf:params:xml:ns:xmpp-sasl'>" + rspAuth.toBase64() + "</challenge>");

							bunny->Authenticating();
							*pBunny = bunny; // Auth OK, set current bunny

							xmpp->currentAuthStep = 3;
							return true;
						}

						LogError(QString("Authentication failure for bunny: %1").arg(QString(username)));
						// Bad password, send failure and restart auth
						answer.append("<failure xmlns='urn:ietf:params:xml:ns:xmpp-sasl'><not-authorized/></failure>");
						xmpp->currentAuthStep = 0;
						return true;
					}
				}
				LogError("Bad Auth Step 2, disconnect");
				return false;
			}

		case 3:
			// We should receive <response xmlns='urn:ietf:params:xml:ns:xmpp-sasl'/>
			if(data.startsWith("<response xmlns='urn:ietf:params:xml:ns:xmpp-sasl'/>"))
			{
				// Send success
				answer.append("<success xmlns='urn:ietf:params:xml:ns:xmpp-sasl'/>");
				xmpp->currentAuthStep = 4;
				return true;
			}
			LogError("Bad Auth Step 3, disconnect");
			return false;

		case 4:
			// We should receive <?xml version='1.0' encoding='UTF-8'?>
			if(data.startsWith("<?xml version='1.0' encoding='UTF-8'?>"))
			{
				// Send success
				answer.append("<?xml version='1.0'?><stream:stream xmlns='jabber:client' xmlns:stream='http://etherx.jabber.org/streams' id='1331400675' from='"+ xmpp->GetXmppDomain() +"' version='1.0' xml:lang='en'>");
				answer.append("<stream:features><bind xmlns='urn:ietf:params:xml:ns:xmpp-bind'><required/></bind><unbind xmlns='urn:ietf:params:xml:ns:xmpp-bind'/><session xmlns='urn:ietf:params:xml:ns:xmpp-session'/></stream:features>");
				xmpp->currentAuthStep = 0;
				(*pBunny)->Authenticated();
				(*pBunny)->SetXmppHandler(xmpp);
				// Bunny is now authenticated
				return true;
			}
			LogError("Bad Auth Step 4, disconnect");
			return false;


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
						Bunny * bunny = BunnyManager::GetBunny(user);
						if(bunny->SetBunnyPassword(ComputeXor(user,password)))
						{
							answer.append(iqAuth.Reply(IQ::Iq_Result, "%1 %2 %3 %4", content));
							xmpp->currentAuthStep = 1;
							return true;
						}
						LogError(QString("Password already set for bunny : ").append(QString(user)));
						return false;
					}
				}
				LogError("Bad Register, disconnect");
				return false;
			}

		default:
			LogError("Unknown Auth Step, disconnect");
			return false;
	}
}

bool PluginAuth::HttpRequestHandle(HTTPRequest & request)
{
	QString uri = request.GetURI();
	if (uri.startsWith("/vl/sendMailXMPP.jsp"))
	{
		QString mac = request.GetArg("m");
		Bunny * b = BunnyManager::GetBunny(this, mac.toAscii());
		b->ClearBunnyPassword();
		LogError("Bunny just call sendMailXMPP, password reset");
		return true;
	}
	return false;
}

/*******/
/* API */
/*******/
void PluginAuth::InitApiCalls()
{
	DECLARE_PLUGIN_API_CALL("setAuthMethod(name)", PluginAuth, Api_SelectAuth);
	DECLARE_PLUGIN_API_CALL("getListOfAuthMethods()", PluginAuth, Api_GetListOfAuths);
}

PLUGIN_API_CALL(PluginAuth::Api_SelectAuth)
{
	Q_UNUSED(hRequest);
	Q_UNUSED(account);

	return new ApiManager::ApiError(QString("This API is deprecated"));
}

PLUGIN_API_CALL(PluginAuth::Api_GetListOfAuths)
{
	Q_UNUSED(hRequest);
	Q_UNUSED(account);

	return new ApiManager::ApiError(QString("This API is deprecated"));
}


