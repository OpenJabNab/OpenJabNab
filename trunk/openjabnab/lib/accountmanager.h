#ifndef _ACCOUNTMANAGER_H_
#define _ACCOUNTMANAGER_H_

#include <QDateTime>
#include <QList>
#include <QMap>
#include "global.h"
#include "account.h"
#include "apihandler.h"
#include "apimanager.h"

typedef struct {
	Account * account;
	unsigned int expire_time;
} TokenData;

class OJN_EXPORT AccountManager : public ApiHandler<AccountManager>
{
	friend class OpenJabNab;
public:
	static AccountManager & Instance();

	Account const& GetAccount(QByteArray const&);
	static Account const& Guest();
	QByteArray GetToken(QString const& login, QByteArray const& hash);

protected:
	static inline void Init() { Instance().LoadAccounts(); Instance().InitApi(); };
	static inline void Close() { Instance().SaveAccounts(); };
	virtual ~AccountManager();

private:
	AccountManager();
	void LoadAccounts();
	void SaveAccounts();
	void InitApi();
	QList<Account *> listOfAccounts;
	QMap<QString, Account *> listOfAccountsByName;
	QMap<QByteArray, TokenData> listOfTokens;

	// API
	ApiManager::ApiAnswer * Api_Auth(Account const&, QString const&, HTTPRequest const&);
	ApiManager::ApiAnswer * Api_RegisterNewAccount(Account const&, QString const&, HTTPRequest const&);
	ApiManager::ApiAnswer * Api_AddBunny(Account const&, QString const&, HTTPRequest const&);
	ApiManager::ApiAnswer * Api_RemoveBunny(Account const&, QString const&, HTTPRequest const&);
};

#endif
