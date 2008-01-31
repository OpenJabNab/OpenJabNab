#ifndef _HTTPHANDLER_H_
#define _HTTPHANDLER_H_

#include <QObject>
#include <QTcpSocket>

#include "pluginmanager.h"

class HttpHandler : public QObject
{
	Q_OBJECT
	
public:
	HttpHandler(QTcpSocket *, PluginManager * p);
	virtual ~HttpHandler();

private slots:
    void handleBunnyHTTPRequest();
	
private:
	QTcpSocket * incomingHttpSocket;
	PluginManager * pluginManager;
};

#endif
