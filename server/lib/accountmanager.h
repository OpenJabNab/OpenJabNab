#ifndef _ACCOUNTMANAGER_H_
#define _ACCOUNTMANAGER_H_

#include <QDateTime>
#include <QList>
#include <QDir>
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
	static Account* GetAccountByLogin(QByteArray const&);
	static Account const& Guest();
	QByteArray GetToken(QString const& login, QByteArray const& hash);

protected:
	static inline void Init();
	static inline void Close();
	virtual ~AccountManager();

private:
	AccountManager();
	void LoadAccounts();
	void SaveAccounts();
	static void InitApiCalls();
	QDir accountsDir;
	QList<Account *> listOfAccounts;
	QHash<QString, Account *> listOfAccountsByName;
	QHash<QByteArray, TokenData> listOfTokens;

	// API
	API_CALL(Api_Auth);
	API_CALL(Api_ChangePasswd);
	API_CALL(Api_RegisterNewAccount);
	API_CALL(Api_AddBunny);
	API_CALL(Api_RemoveBunny);
    API_CALL(Api_RemoveZtamp);
	API_CALL(Api_SetToken);
	API_CALL(Api_SetAdmin);
	API_CALL(Api_GetUserInfos);
	API_CALL(Api_GetUserlist);
	API_CALL(Api_GetConnectedUsers);
	API_CALL(Api_GetListOfAdmins);
};

inline void AccountManager::Init()
{
	Instance().LoadAccounts();
	InitApiCalls();
}

inline void AccountManager::Close()
{
	Instance().SaveAccounts();
}

#endif
