#ifndef _ACCOUNT_H_
#define _ACCOUNT_H_

#include <QByteArray>
#include <QDataStream>
#include <QFlags>
#include <QList>
#include <QString>
#include "apimanager.h"
#include "apihandler.h"
#include "global.h"

class OJN_EXPORT Account : public ApiHandler<Account>
{
	friend class AccountManager;
public:
	enum SpecialAccount { Guest, DefaultAdmin };
	enum Right { None = 0x0, Read = 0x1, Write = 0x2};
	Q_DECLARE_FLAGS(Rights, Right);

	static void Init() { InitApiCalls(); }

	QByteArray const& GetPasswordHash() const;
	QString const& GetLogin() const;
	QByteArray const& GetToken() const;
	void SetToken(QByteArray);
	bool IsAdmin() const;
	bool HasPluginsAccess(Right r) const;
	bool HasBunniesAccess(Right r) const;
	bool HasZtampsAccess(Right r) const;
	bool HasGlobalAccess(Right r) const;
	bool HasGeneralPluginAccess(Right r) const;
	bool HasBunnyAccess(QByteArray const& b) const;
	bool HasZtampAccess(QByteArray const& b) const;
	static int Version();

private:
	Account();
	Account(SpecialAccount t);
	Account(QDataStream & in, unsigned int version);
	Account(QString const& login, QString const& username, QByteArray const& passwordHash);

	void SetDefault();
	QByteArray AddBunny(QByteArray const& b);
	bool RemoveBunny(QByteArray const& b);
	QByteArray AddZtamp(QByteArray const& z);
	bool RemoveZtamp(QByteArray const& z);

	static void InitApiCalls();

	QString login;
	QString username;
	QByteArray passwordHash;
	QByteArray token;
	bool isAdmin;
	Rights PluginsAccess;
	Rights BunniesAccess;
	Rights ZtampsAccess;
	Rights GlobalAccess;
	Rights GeneralPluginAccess;
	QList<QByteArray> listOfBunnies;
	QList<QByteArray> listOfZtamps;

	// API
	API_CALL(Api_Info);

	friend QDataStream & operator<< (QDataStream & out, const Account & a);
};

// Account => QDataStream
extern QDataStream & operator<< (QDataStream & out, const Account &);

// Account::Rights <=> QDataStream
extern QDataStream & operator<< (QDataStream & out, const Account::Rights &);
extern QDataStream & operator>> (QDataStream & in, Account::Rights &);

// Inline Public methods
inline QByteArray const& Account::GetPasswordHash() const 
{
	return passwordHash;
}

inline QString const& Account::GetLogin() const
{
	return login;
}

inline QByteArray const& Account::GetToken() const
{
	return token;
}

inline void Account::SetToken(QByteArray t)
{
	token = t;
}

inline bool Account::IsAdmin() const
{
	return isAdmin;
}

inline bool Account::HasPluginsAccess(Right r) const
{
	if(isAdmin)
		return true;
	return PluginsAccess.testFlag(r);
}

inline bool Account::HasZtampsAccess(Right r) const
{
	if(isAdmin)
		return true;
	return ZtampsAccess.testFlag(r);
}

inline bool Account::HasBunniesAccess(Right r) const
{
	if(isAdmin)
		return true;
	return BunniesAccess.testFlag(r);
}

inline bool Account::HasGlobalAccess(Right r) const 
{
	if(isAdmin)
		return true;
	return GlobalAccess.testFlag(r);
}

inline bool Account::HasGeneralPluginAccess(Right r) const
{
	if(isAdmin)
		return true;
	return GeneralPluginAccess.testFlag(r);
}

inline bool Account::HasZtampAccess(QByteArray const& b) const
{
	if(isAdmin)
		return true;
	return listOfZtamps.contains(QByteArray::fromHex(b));
}

inline bool Account::HasBunnyAccess(QByteArray const& b) const
{
	if(isAdmin)
		return true;
	return listOfBunnies.contains(QByteArray::fromHex(b));
}

inline int Account::Version() {
	return 1;
}

// Inline protected methods
inline QByteArray Account::AddBunny(QByteArray const& b)
{
		QByteArray id = QByteArray::fromHex(b);
		listOfBunnies.append(id);
		return id.toHex();
}

inline bool Account::RemoveBunny(QByteArray const& b)
{
	QByteArray id = QByteArray::fromHex(b);
	return (listOfBunnies.removeAll(id) != 0);
}

inline QByteArray Account::AddZtamp(QByteArray const& z)
{
		QByteArray id = QByteArray::fromHex(z);
		listOfZtamps.append(id);
		return id.toHex();
}

inline bool Account::RemoveZtamp(QByteArray const& z)
{
	QByteArray id = QByteArray::fromHex(z);
	return (listOfZtamps.removeAll(id) != 0);
}

#endif
