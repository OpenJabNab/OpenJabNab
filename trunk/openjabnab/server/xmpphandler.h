#ifndef _XMPPHANDLER_H_
#define _XMPPHANDLER_H_

#include <QObject>
#include <QTcpSocket>

class XmppHandler : public QObject
{
	Q_OBJECT
	
public:
	XmppHandler(QTcpSocket *);

protected:
	virtual ~XmppHandler();

private slots:
	void handleBunnyXmppMessage();
	void handleVioletXmppMessage();

private:
	QTcpSocket * incomingXmppSocket;
	QTcpSocket * outgoingXmppSocket;
};

#endif
