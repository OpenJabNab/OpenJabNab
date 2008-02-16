#include <QHttp>
#include "httprequest.h"

HTTPRequest::HTTPRequest(QString const& _uri):uri(_uri)
{
}

void HTTPRequest::ForwardDone(bool)
{
	loop.exit();
}

QByteArray HTTPRequest::ForwardTo(QString const& server)
{
	QByteArray answer;
	QHttp http(server);
	connect(&http, SIGNAL(done(bool)), this, SLOT(ForwardDone(bool)));
	http.get(uri);
	loop.exec();
	answer = http.readAll();
	http.close();
	return answer;
}

QStringList HTTPRequest::GetArgs() const
{
	if (uri.contains('?'))
	{
		return QString(uri).remove(0, uri.indexOf('?')+1).split('&');
	}
	else
		return QStringList();
}
