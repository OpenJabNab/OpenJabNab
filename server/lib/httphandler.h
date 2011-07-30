#ifndef _HTTPHANDLER_H_
#define _HTTPHANDLER_H_

#include <QObject>
#include <QTcpSocket>
#include "global.h"

class PluginManager;
class ApiManager;
class VioletApiManager;
class OJN_EXPORT HttpHandler : public QObject
{
	Q_OBJECT

public:
	HttpHandler(QTcpSocket *, bool, bool, bool, bool);
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
	bool httpVioletApi;
	bool httpViolet;
	QByteArray receivedData;
	int bytesToReceive;
	bool isStandAlone;
};

#endif
