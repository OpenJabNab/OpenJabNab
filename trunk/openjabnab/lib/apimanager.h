#ifndef _APIMANAGER_H_
#define _APIMANAGER_H_

#include <QByteArray>
#include <QString>
#include <QList>
#include "global.h"

class HTTPRequest;
class PluginManager;
class OJN_EXPORT ApiManager
{
public:
	class OJN_EXPORT ApiAnswer
	{
		public:
			virtual ~ApiAnswer() {}
			virtual QByteArray GetData();
			virtual QByteArray GetInternalData() = 0;

		protected:
			QByteArray SanitizeXML(QByteArray const&);
	};

	ApiManager(PluginManager * p);
	ApiAnswer * ProcessApiCall(QByteArray const&, HTTPRequest const&);
	
	// Internal classes
	class OJN_EXPORT ApiError : public ApiAnswer
	{
		public:
			ApiError(QByteArray s):error(s) {}
			ApiError(QString s):error(s.toAscii()) {}
			QByteArray GetInternalData() { return "<error>" + SanitizeXML(error) + "</error>"; }
		private:
			QByteArray error;
	};

	class OJN_EXPORT ApiString : public ApiAnswer
	{
		public:
			ApiString(QByteArray s):string(s) {}
			ApiString(QString s):string(s.toAscii()) {}
			QByteArray GetInternalData() { return "<value>" + SanitizeXML(string) + "</value>"; }
		private:
			QByteArray string;
	};

	class OJN_EXPORT ApiList : public ApiAnswer
	{
		public:
			ApiList(QList<QByteArray> l):list(l) {}
			QByteArray GetInternalData() { 
				QByteArray tmp;
				tmp += "<list>";
				foreach (QByteArray b, list)
					tmp += "<item>" + SanitizeXML(b) + "</item>";
				tmp += "</list>";
				return tmp;
			}
		private:
			QList<QByteArray> list;
	};

private:
	ApiAnswer * ProcessGlobalApiCall(QByteArray const&, HTTPRequest const&);
	ApiAnswer * ProcessPluginApiCall(QByteArray const&, HTTPRequest const&);
	ApiAnswer * ProcessBunnyApiCall(QByteArray const&, HTTPRequest const&);
	
	PluginManager * pluginManager;
};
#endif
