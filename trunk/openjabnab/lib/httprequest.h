#ifndef _HTTPREQUEST_H_
#define _HTTPREQUEST_H_

#include <QMap>
#include <QString>
#include <QStringList>
#include "global.h"

class OJN_EXPORT HTTPRequest
{

public:
	enum RequestType { INVALID, GET, POST, POSTRAW };

	HTTPRequest(QByteArray const&);
	QByteArray ForwardTo(QString const& server);
	inline QByteArray const& GetURI() const { return uri; };
	inline QByteArray const& GetRawURI() const { return rawUri; };
	inline QString GetArg(QString const& s) const { return getData.value(s, QString()); };
	inline bool HasArg(QString const& s) const { return getData.contains(s); };
	inline void RemoveArg(QString const& s) { getData.remove(s); };
	inline QString GetPostArg(QString const& s) const { return formPostData.value(s, QString()); };
	inline bool HasPostArg(QString const& s) const { return formPostData.contains(s); };
	inline QMap<QString, QString> const& GetArgs() const { return getData; };
	inline QMap<QString, QString> const& GetPost() const { return formPostData; };
	inline QByteArray const& GetRawPost() const { return rawPostData; };
	inline RequestType const& GetType() const { return type; };
	QString toString() const;
	
	QByteArray reply;

private:
	QByteArray rawUri;
	QByteArray rawHeaders;
	QByteArray rawPostData;
	QByteArray uri;
	QMap<QString, QString> getData;
	QMap<QString, QString> formPostData;
	RequestType type;
};

#endif
