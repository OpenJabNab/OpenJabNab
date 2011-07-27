#ifndef _PLUGINXMPPPORT_H_
#define _PLUGINXMPPPORT_H_

#include "plugininterface.h"

class PluginXmppport : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	PluginXmppport();
	virtual ~PluginXmppport();
	void HttpRequestAfter(HTTPRequest &);

	void InitApiCalls();
	PLUGIN_API_CALL(Api_setXmppPort);
	PLUGIN_API_CALL(Api_getXmppPort);

private:
	void patchBootcode(HTTPRequest &, long, int, QByteArray, QByteArray);
	QByteArray convertLSBFirst(int);
};

#endif
