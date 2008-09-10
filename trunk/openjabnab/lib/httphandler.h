#ifndef _HTTPHANDLER_H_
#define _HTTPHANDLER_H_

#include <QObject>
#include <QTcpSocket>
#include "global.h"

class PluginManager;
class ApiManager;
class OJN_EXPORT HttpHandler : public QObject
{
	Q_OBJECT
	
public:
	HttpHandler(QTcpSocket *, PluginManager *, ApiManager *, bool, bool);
	virtual ~HttpHandler();

private slots:
    void HandleBunnyHTTPRequest();
	
private:
	QTcpSocket * incomingHttpSocket;
	PluginManager * pluginManager;
	ApiManager * apiManager;
	bool httpApi;
	bool httpViolet;
};

#endif
