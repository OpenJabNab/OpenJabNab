#ifndef _APIMANAGER_H_
#define _APIMANAGER_H_

#include <QByteArray>
#include <QList>
#include "global.h"
#include "pluginmanager.h"

class OJN_EXPORT ApiManager
{
public:
	class ApiAnswer
	{
		public:
			virtual ~ApiAnswer() {}
			virtual QByteArray GetData();
			virtual QByteArray GetInternalData() = 0;

		protected:
			QByteArray SanitizeXML(QByteArray const&);
	};

	ApiManager(PluginManager * p);
	ApiAnswer * ProcessApiCall(QByteArray const&);
	
private:
	ApiAnswer * ProcessGlobalApiCall(QByteArray const&);
	ApiAnswer * ProcessPluginApiCall(QByteArray const&);
	ApiAnswer * ProcessBunnyApiCall(QByteArray const&);
	
	PluginManager * pluginManager;
	
	class ApiError : public ApiAnswer
	{
		public:
			ApiError(QByteArray s):error(s) {}
			QByteArray GetInternalData() { return "<error>" + SanitizeXML(error) + "</error>"; }
		private:
			QByteArray error;
	};

	class ApiString : public ApiAnswer
	{
		public:
			ApiString(QByteArray s):string(s) {}
			QByteArray GetInternalData() { return "<value>" + SanitizeXML(string) + "</value>"; }
		private:
			QByteArray string;
	};

	class ApiList : public ApiAnswer
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
};
#endif
