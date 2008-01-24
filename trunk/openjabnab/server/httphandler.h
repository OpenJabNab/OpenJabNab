#ifndef _HTTPHANDLER_H_
#define _HTTPHANDLER_H_

#include <QObject>
#include <QTcpSocket>

class HttpHandler : public QObject
{
	Q_OBJECT
	
public:
	HttpHandler(QTcpSocket *);
	virtual ~HttpHandler();

private slots:
    void handleBunnyHTTPRequest();
	
private:
	QTcpSocket * incomingHttpSocket;
};

#endif
