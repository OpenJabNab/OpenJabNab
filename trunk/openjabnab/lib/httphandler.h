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
	HttpHandler(QTcpSocket *, bool, bool);
	virtual ~HttpHandler();

public slots:
	void Disconnect();

private slots:
	void ReceiveData();
	
private:
    void HandleBunnyHTTPRequest();
	QTcpSocket * incomingHttpSocket;
	PluginManager & pluginManager;
	bool httpApi;
	bool httpViolet;
	QByteArray receivedData;
	int bytesToReceive;
};

#endif
