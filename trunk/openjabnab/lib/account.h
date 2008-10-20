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
	inline QString const& GetLogin() const { return login; }
	inline bool IsAdmin() const { return isAdmin; }
	inline bool HasPluginsAccess(Right r) const { return (isAdmin || PluginsAccess.testFlag(r)); }
	inline bool HasBunniesAccess(Right r) const { return (isAdmin || BunniesAccess.testFlag(r)); }
	inline bool HasGlobalAccess(Right r) const { return (isAdmin || GlobalAccess.testFlag(r)); }
	inline bool HasGeneralPluginAccess(Right r) const { return (isAdmin || GeneralPluginAccess.testFlag(r)); }
	inline bool HasBunnyAccess(QByteArray const& b) const { return listOfBunnies.contains(QByteArray::fromHex(b)); }
	inline static int Version() { return 1; }


private:
	Account();
	Account(SpecialAccount t);
	Account(QDataStream & in, unsigned int version);
	Account(QString const& login, QString const& username, QByteArray const& passwordHash);

	void SetDefault();
	inline QByteArray AddBunny(QByteArray const& b) { QByteArray id = QByteArray::fromHex(b); listOfBunnies.append(id); return id.toHex(); }

	QString login;
	QString username;
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
