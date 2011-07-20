#ifndef _APIMANAGER_H_
#define _APIMANAGER_H_

#include <QByteArray>
#include <QList>
#include <QMap>
#include <QMapIterator>
#include <QString>
#include <QVariant>
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
			QString GetInternalData();
		private:
			QString error;
	};

	class OJN_EXPORT ApiXml : public ApiAnswer
	{
		public:
			ApiXml():string(QString()) {}
			ApiXml(QString s):string(s) {}
			QString GetInternalData() { return string; }
		private:
			QString string;
	};

	class OJN_EXPORT ApiOk : public ApiAnswer
	{
		public:
			ApiOk():string(QString()) {}
			ApiOk(QString s):string(s) {}
			QString GetInternalData();
		private:
			QString string;
	};

	class OJN_EXPORT ApiString : public ApiAnswer
	{
		public:
			ApiString(QString s):string(s) {}
			QString GetInternalData();
		private:
			QString string;
	};

	class OJN_EXPORT ApiList : public ApiAnswer
	{
		public:
			ApiList(QList<QString> l):list(l) {}
			QString GetInternalData();
		private:
			QList<QString> list;
	};

	class OJN_EXPORT ApiMappedList : public ApiAnswer
	{
		public:
			ApiMappedList(QMap<QString, QVariant> l):list(l) {}
			QString GetInternalData();
		private:
			QMap<QString, QVariant> list;
	};

	class OJN_EXPORT ApiViolet : public ApiAnswer
	{
		public:
			ApiViolet(QString m, QString c) { AddMessage(m, c); }
			ApiViolet():string(QString()) {}
			ApiViolet(QString s):string(s) {}
			QByteArray GetData();
			void AddMessage(QString, QString);
			void AddEarPosition(int, int);
			void AddXml(QString s) { string += s; }
			QString GetInternalData() { return string; }
		private:
			QString string;
	};

private:
	ApiManager();
	ApiAnswer * ProcessGlobalApiCall(Account const&, QString const&, HTTPRequest const&);
	ApiAnswer * ProcessPluginApiCall(Account const&, QString const&, HTTPRequest &);
	ApiAnswer * ProcessBunnyApiCall(Account const&, QString const&, HTTPRequest const&);
	ApiAnswer * ProcessZtampApiCall(Account const&, QString const&, HTTPRequest const&);
	ApiAnswer * ProcessBunnyVioletApiCall(QString const&, HTTPRequest const&);
};
#endif
