#ifndef _XMPPHANDLER_H_
#define _XMPPHANDLER_H_

#include <QObject>
#include <QTcpSocket>

#include "pluginmanager.h"

class XmppHandler : public QObject
{
	Q_OBJECT
	
public:
	XmppHandler(QTcpSocket *, PluginManager * p);

protected:
	virtual ~XmppHandler() {};

private slots:
	void handleBunnyXmppMessage();
	void handleVioletXmppMessage();
	void onDisconnect();

private:
	QTcpSocket * incomingXmppSocket;
	QTcpSocket outgoingXmppSocket;
	PluginManager * pluginManager;
};

#endif
