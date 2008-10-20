#ifndef _ACCOUNTMANAGER_H_
#define _ACCOUNTMANAGER_H_

#include <QDateTime>
#include <QList>
#include <QMap>
#include "global.h"
#include "account.h"
#include "apimanager.h"

typedef struct {
	Account * account;
	unsigned int expire_time;
} TokenData;

class OJN_EXPORT AccountManager
{
	friend class OpenJabNab;
public:
	static AccountManager & Instance();

	Account const& GetAccount(QByteArray const&);
	static Account const& Guest();
	QByteArray GetToken(QString const& login, QByteArray const& hash);
	ApiManager::ApiAnswer * ProcessApiCall(Account const&, QString const& request, HTTPRequest const& hRequest);

protected:
	static inline void Init() { Instance().LoadAccounts(); };
	static inline void Close() { Instance().SaveAccounts(); };
	virtual ~AccountManager();

private:
	AccountManager();
	void LoadAccounts();
	void SaveAccounts();
	QList<Account *> listOfAccounts;
	QMap<QString, Account *> listOfAccountsByName;
	QMap<QByteArray, TokenData> listOfTokens;
};

#endif
