#ifndef _ACCOUNTMANAGER_H_
#define _ACCOUNTMANAGER_H_

#include <QDateTime>
#include <QMap>
#include <QVector>
#include "global.h"
#include "account.h"
#include "apimanager.h"

class OJN_EXPORT AccountManager
{
public:
	static AccountManager & Instance();
	virtual ~AccountManager();

	QVector<Account *> const& GetListOfAccounts() { return listOfAccounts; }
	Account * GetAccountByName(QString name) { return listOfAccountsByName.value(name); }
	QByteArray GenerateNewToken(Account *);
	ApiManager::ApiAnswer * ProcessApiCall(QByteArray const& request, HTTPRequest const& hRequest);
	bool checkTokenValidity(QByteArray, Account *);
	void updateToken(QByteArray);


private:
	AccountManager();
	QByteArray Login(QString const&, QByteArray const&);
	QVector<Account *> listOfAccounts;
	QMap<QString, Account *> listOfAccountsByName;
	QMap<QByteArray, QDateTime> listOfTokenExpiration;
	QMap<QByteArray, Account *> listOfToken;
};

#endif
