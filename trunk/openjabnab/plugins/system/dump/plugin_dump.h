#ifndef _PLUGINDUMP_H_
#define _PLUGINDUMP_H_

#include <QTextStream>
#include "plugininterface.h"
#include "httprequest.h"
	
class PluginDump : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	PluginDump();
	virtual ~PluginDump();
	void HttpRequestBefore(HTTPRequest const&);

	void XmppBunnyMessage(Bunny *, QByteArray const&);
	void XmppVioletMessage(Bunny *, QByteArray const&);
	bool XmppVioletPacketMessage(Bunny *, Packet const&);
	
private:
	QTextStream dumpStream;

};

#endif
