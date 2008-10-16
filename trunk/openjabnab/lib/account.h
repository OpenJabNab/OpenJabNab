#ifndef _ACCOUNT_H_
#define _ACCOUNT_H_

#include <QByteArray>
#include <QDataStream>
#include <QFlags>
#include <QList>
#include <QString>
#include "apimanager.h"
#include "global.h"

class OJN_EXPORT Account
{
	friend class AccountManager;
public:
	enum SpecialAccount { Guest, DefaultAdmin };
	enum Right { None = 0x0, Read = 0x1, Write = 0x2};
	Q_DECLARE_FLAGS(Rights, Right);

	inline QByteArray const& GetPasswordHash() const { return passwordHash; }
	inline void SetPassword(QByteArray const& b) { passwordHash = b; }
	inline QByteArray const& GetLogin() const { return login; }
	inline bool IsAdmin() const { return isAdmin; }
	inline bool HasPluginsAccess(Right r) const { return (isAdmin || PluginsAccess.testFlag(r)); }
	inline bool HasBunniesAccess(Right r) const { return (isAdmin || BunniesAccess.testFlag(r)); }
	inline bool HasGlobalAccess(Right r) const { return (isAdmin || GlobalAccess.testFlag(r)); }
	inline bool HasGeneralPluginAccess(Right r) const { return (isAdmin || GeneralPluginAccess.testFlag(r)); }
	inline bool HasBunnyAccess(QByteArray const& b) const { return listOfBunnies.contains(QByteArray::fromHex(b)); }
	inline static int Version() { return 1; }


private:
	Account();
	Account::Account(SpecialAccount t);
	Account::Account(QDataStream & in, unsigned int version);
	Account::Account(QByteArray const& login, QByteArray const& username, QByteArray const& passwordHash);

	void SetDefault();

	QByteArray login;
	QByteArray username;
	QByteArray passwordHash;
	bool isAdmin;
	Rights PluginsAccess;
	Rights BunniesAccess;
	Rights GlobalAccess;
	Rights GeneralPluginAccess;
	QList<QByteArray> listOfBunnies;

	friend QDataStream & operator<< (QDataStream & out, const Account & a);
};

extern QDataStream & operator<< (QDataStream & out, const Account &);

extern QDataStream & operator<< (QDataStream & out, const Account::Rights &);
extern QDataStream & operator>> (QDataStream & in, Account::Rights &);
#endif
