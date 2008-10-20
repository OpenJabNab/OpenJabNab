#ifndef _APIMANAGER_H_
#define _APIMANAGER_H_

#include <QByteArray>
#include <QList>
#include <QMap>
#include <QMapIterator>
#include <QString>
#include "global.h"

class Account;
class AccountManager;
class HTTPRequest;
class PluginManager;
class OJN_EXPORT ApiManager
{
public:
	class OJN_EXPORT ApiAnswer
	{
		public:
			virtual ~ApiAnswer() {}
			virtual QByteArray GetData(); // UTF8
			virtual QString GetInternalData() = 0;

		protected:
			QString SanitizeXML(QString const&);
	};

	static ApiManager & Instance();
	ApiAnswer * ProcessApiCall(QString const&, HTTPRequest &);
	
	// Internal classes
	class OJN_EXPORT ApiError : public ApiAnswer
	{
		public:
			ApiError(QString s):error(s) {}
			QString GetInternalData() { return QString("<error>%1</error>").arg(SanitizeXML(error)); }
		private:
			QString error;
	};

	class OJN_EXPORT ApiOk : public ApiAnswer
	{
		public:
			ApiOk():string(QString()) {}
			ApiOk(QString s):string(s) {}
			QString GetInternalData() { return QString("<ok>%1</ok>").arg(SanitizeXML(string)); }
		private:
			QString string;
	};

	class OJN_EXPORT ApiString : public ApiAnswer
	{
		public:
			ApiString(QString s):string(s) {}
			QString GetInternalData() { return QString("<value>%1</value>").arg(SanitizeXML(string)); }
		private:
			QString string;
	};

	class OJN_EXPORT ApiList : public ApiAnswer
	{
		public:
			ApiList(QList<QString> l):list(l) {}
			QString GetInternalData() { 
				QString tmp;
				tmp += "<list>";
				foreach (QString b, list)
					tmp += QString("<item>%1</item>").arg(SanitizeXML(b));
				tmp += "</list>";
				return tmp;
			}
		private:
			QList<QString> list;
	};

	class OJN_EXPORT ApiMappedList : public ApiAnswer
	{
		public:
			ApiMappedList(QMap<QString, QString> l):list(l) {}
			QString GetInternalData() { 
				QString tmp;
				tmp += "<list>";
				QMapIterator<QString, QString> i(list);
				while (i.hasNext()) {
					i.next();
					tmp += QString("<item><key>%1</key><value>%2</value></item>").arg(SanitizeXML(i.key()), SanitizeXML(i.value()));
				}
				tmp += "</list>";
				return tmp;
			}
		private:
			QMap<QString, QString> list;
	};

private:
	ApiManager();
	ApiAnswer * ProcessGlobalApiCall(Account const&, QString const&, HTTPRequest const&);
	ApiAnswer * ProcessPluginApiCall(Account const&, QString const&, HTTPRequest &);
	ApiAnswer * ProcessBunnyApiCall(Account const&, QString const&, HTTPRequest const&);
};
#endif
