#ifndef _HTTPREQUEST_H_
#define _HTTPREQUEST_H_

#include <QHash>
#include <QString>
#include <QStringList>
#include "global.h"

class OJN_EXPORT HTTPRequest
{

public:
	enum RequestType { INVALID, GET, POST, POSTRAW };

	HTTPRequest(QByteArray const&);
	QByteArray ForwardTo(QString const& server);
	QString const& GetURI() const;
	QByteArray const& GetRawURI() const;
	QString GetArg(QString const& s) const;
	bool HasArg(QString const& s) const;
	void RemoveArg(QString const& s);
	QString GetPostArg(QString const& s) const;
	bool HasPostArg(QString const& s) const;
	QHash<QString, QString> const& GetArgs() const;
	QHash<QString, QString> const& GetPost() const;
	QByteArray const& GetRawPost() const;
	RequestType const& GetType() const;
	QString toString() const;
	
	QByteArray reply;

private:
	QByteArray rawUri;
	QByteArray rawHeaders;
	QByteArray rawPostData;
	QString uri;
	QHash<QString, QString> getData;
	QHash<QString, QString> formPostData;
	RequestType type;
};

inline QString const& HTTPRequest::GetURI() const
{
	return uri;
}

inline QByteArray const& HTTPRequest::GetRawURI() const
{
	return rawUri;
}

inline bool HTTPRequest::HasArg(QString const& s) const 
{
	return getData.contains(s);
}

inline QString HTTPRequest::GetArg(QString const& s) const
{
	return getData.value(s, QString());
}


inline void HTTPRequest::RemoveArg(QString const& s)
{
	getData.remove(s);
}

inline QString HTTPRequest::GetPostArg(QString const& s) const
{
	return formPostData.value(s, QString());
}

inline bool HTTPRequest::HasPostArg(QString const& s) const
{
	return formPostData.contains(s);
}

inline QHash<QString, QString> const& HTTPRequest::GetArgs() const
{
	return getData;
}

inline QHash<QString, QString> const& HTTPRequest::GetPost() const
{
	return formPostData;
}

inline QByteArray const& HTTPRequest::GetRawPost() const
{
	return rawPostData;
}

inline HTTPRequest::RequestType const& HTTPRequest::GetType() const
{
	return type; 
}
#endif
