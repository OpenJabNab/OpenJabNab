#ifndef _HTTPHANDLER_H_
#define _HTTPHANDLER_H_

#include <QObject>
#include <QTcpSocket>

#include "pluginmanager.h"
#include "global.h"

class OJN_EXPORT HttpHandler : public QObject
{
	Q_OBJECT
	
public:
	HttpHandler(QTcpSocket *, PluginManager * p);
	virtual ~HttpHandler();

private slots:
    void HandleBunnyHTTPRequest();
	
private:
	QTcpSocket * incomingHttpSocket;
	PluginManager * pluginManager;
};

#endif
