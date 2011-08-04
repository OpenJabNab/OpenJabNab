#ifndef _PLUGINAUTH_H_
#define _PLUGINAUTH_H_

#include "pluginauthinterface.h"
#include "httprequest.h"

#include <QMap>

class PluginAuth : public PluginAuthInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	PluginAuth();
	virtual ~PluginAuth() {};

	virtual bool DoAuth(XmppHandler * xmpp, QByteArray const& data, Bunny ** pBunny, QByteArray & answer);
	void HttpRequestAfter(HTTPRequest &);

	void InitApiCalls();
	PLUGIN_API_CALL(Api_SelectAuth);
	PLUGIN_API_CALL(Api_GetListOfAuths);
	
private:
	static bool DummyAuth(XmppHandler * xmpp, QByteArray const& data, Bunny ** pBunny, QByteArray & answer);
	static bool FullAuth(XmppHandler * xmpp, QByteArray const& data, Bunny ** pBunny, QByteArray & answer);
	static bool PatchedAuth(XmppHandler * xmpp, QByteArray const& data, Bunny ** pBunny, QByteArray & answer);

	typedef bool (*pAuthFunction)(XmppHandler *, QByteArray const&, Bunny **, QByteArray &);

	pAuthFunction currentAuthFunction;
	
	QMap<QString, pAuthFunction> listOfAuthFunctions;
};

#endif
