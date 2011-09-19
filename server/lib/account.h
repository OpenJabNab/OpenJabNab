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
	enum Right { None = 0x0, Read = 0x1, Write = 0x2, ReadWrite = 0x3};
	enum Access { AcGlobal = 0x0, AcAccount = 0x1, AcBunnies = 0x2, AcZtamps = 0x3, AcPluginsBunny = 0x4, AcPluginsZtamp = 0x5, AcPlugins= 0x6, AcServer = 0x7};
	Q_DECLARE_FLAGS(Rights, Right);

	static void Init() { InitApiCalls(); }

	QByteArray const& GetPasswordHash() const;
	void SetPassword(QByteArray const& p);
	QString const& GetLogin() const;
	QString const& GetUsername() const;
	QByteArray const& GetToken() const;
	QString const& GetLanguage() const;
	void SetLanguage(QString const& lng);
	void SetToken(QByteArray);
	bool IsAdmin() const;
	void setAdmin();
	bool HasAccess(Access id, Right r) const;
	void SetAccess(Access id,Right r);
	bool HasBunnyAccess(QByteArray const& b) const;
	bool HasZtampAccess(QByteArray const& b) const;
	QList<QByteArray> const& GetBunniesList() const;
	QList<QByteArray> const& GetZtampsList() const;
	static int Version();
	QByteArray AddZtamp(QByteArray const& z);

private:
	Account();
	Account(SpecialAccount t);
	Account(QDataStream & in, unsigned int version);
	Account(QString const& login, QString const& username, QByteArray const& passwordHash);
	Account(QString const& login, QString const& username, QByteArray const& passwordHash, QString const& language);

	void SetDefault();
	QByteArray AddBunny(QByteArray const& b);
	bool RemoveBunny(QByteArray const& b);
	bool RemoveZtamp(QByteArray const& z);

	static void InitApiCalls();

	QString login;
	QString username;
	QByteArray passwordHash;
	QByteArray token;
	QString language;

	bool isAdmin;
	QList<Rights> UserAccess;

	QList<QByteArray> listOfBunnies;
	QList<QByteArray> listOfZtamps;

	friend QDataStream & operator<< (QDataStream & out, const Account & a);
};

// Account => QDataStream
extern QDataStream & operator<< (QDataStream & out, const Account &);

// Account::Rights <=> QDataStream
extern QDataStream & operator<< (QDataStream & out, const Account::Rights &);
extern QDataStream & operator>> (QDataStream & in, Account::Rights &);

// Inline Public methods
inline QList<QByteArray> const& Account::GetBunniesList() const {
	return listOfBunnies;
}

inline QList<QByteArray> const& Account::GetZtampsList() const {
	return listOfZtamps;
}

inline QByteArray const& Account::GetPasswordHash() const
{
	return passwordHash;
}

inline void Account::SetPassword(QByteArray const& p) {
	passwordHash = p;
}

inline QString const& Account::GetLogin() const
{
	return login;
}

inline QString const& Account::GetUsername() const
{
	return username;
}

inline QString const& Account::GetLanguage() const
{
	return language;
}

inline void Account::SetLanguage(QString const& lng) {
	language = lng;
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

inline void Account::setAdmin() {
	isAdmin = true;
}

inline bool Account::HasAccess(Access id,Right r) const
{
	if(isAdmin)
		return true;
	return UserAccess[id].testFlag(r);
}

inline void Account::SetAccess(Access id,Right r)
{
	UserAccess[id] = r;
}

inline bool Account::HasZtampAccess(QByteArray const& b) const
{
	if(isAdmin)
		return true;
	return listOfZtamps.contains(b);
}

inline bool Account::HasBunnyAccess(QByteArray const& b) const
{
	if(isAdmin)
		return true;
	return listOfBunnies.contains(b);
}

inline int Account::Version() {
	return 1;
}

// Inline protected methods
inline QByteArray Account::AddBunny(QByteArray const& b) {
	if(!listOfBunnies.contains(b))
		listOfBunnies.append(b);
	return b;
}

inline bool Account::RemoveBunny(QByteArray const& b)
{
	return (listOfBunnies.removeAll(b) != 0);
}

inline QByteArray Account::AddZtamp(QByteArray const& z)
{
	if(!listOfZtamps.contains(z))
		listOfZtamps.append(z);
	return z;
}

inline bool Account::RemoveZtamp(QByteArray const& z)
{
	return (listOfZtamps.removeAll(z) != 0);
}

#endif
