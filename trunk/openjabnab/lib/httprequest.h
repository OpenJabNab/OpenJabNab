#ifndef _HTTPREQUEST_H_
#define _HTTPREQUEST_H_

#include <QEventLoop>
#include <QObject>
#include <QString>
#include <QStringList>
#include "global.h"

class OJN_EXPORT HTTPRequest : public QObject
{
	Q_OBJECT

public:
	HTTPRequest(QString const&);
	QByteArray ForwardTo(QString const& server);
	QString const& GetURI() const { return uri; };
	QStringList GetArgs() const;
	
	QByteArray reply;

private slots:
	void ForwardDone(bool);

private:
	QString uri;
	QEventLoop loop;
};

#endif
