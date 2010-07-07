#ifndef _PLUGINAUTH_H_
#define _PLUGINAUTH_H_

#include "pluginauthinterface.h"
#include "httprequest.h"
	
class PluginAuth : public PluginAuthInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	PluginAuth();
	virtual ~PluginAuth() {};

	virtual bool DoAuth(XmppHandler * xmpp, QByteArray const& data, Bunny ** pBunny, QByteArray & answer);
};

#endif
