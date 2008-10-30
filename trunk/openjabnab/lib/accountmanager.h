#ifndef _ACCOUNTMANAGER_H_
#define _ACCOUNTMANAGER_H_

#include <QDateTime>
#include <QList>
#include <QHash>
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

	// API
	static void InitApiCalls();

protected:
	static inline void Init() { Instance().LoadAccounts(); };
	static inline void Close() { Instance().SaveAccounts(); };
	virtual ~AccountManager();

private:
	AccountManager();
	void LoadAccounts();
	void SaveAccounts();
	QList<Account *> listOfAccounts;
	QHash<QString, Account *> listOfAccountsByName;
	QHash<QByteArray, TokenData> listOfTokens;

	// API
	API_CALL(Api_Auth);
	API_CALL(Api_RegisterNewAccount);
	API_CALL(Api_AddBunny);
	API_CALL(Api_RemoveBunny);
};

#endif
