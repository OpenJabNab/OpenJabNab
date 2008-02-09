#ifndef _PLUGININTERFACE_H_
#define _PLUGININTERFACE_H_

#include <QByteArray>
#include <QtPlugin>
#include "httprequest.h"
#include "packet.h"

class PluginInterface
{
public:
	enum ClickType { SingleClick = 0, DoubleClick};

	virtual ~PluginInterface() {};

	virtual void HttpRequestBefore(HTTPRequest const&) {};
	// If the plugin returns true, the plugin should handle the request
	virtual bool HttpRequestHandle(HTTPRequest &) { return false; };
	virtual void HttpRequestAfter(HTTPRequest const&) {};
	
	virtual void XmppBunnyMessage(QByteArray const&) {};
	virtual void XmppVioletMessage(QByteArray const&) {};
	// If the plugin returns true, the packet will be dropped
	virtual bool XmppVioletPacketMessage(Packet const&) { return false; };

	virtual bool OnClick(ClickType) { return false; };
	virtual bool OnEarsMove(int, int) { return false; };
};

Q_DECLARE_INTERFACE(PluginInterface,"org.toms.openjabnab.PluginInterface/1.0")

#endif
