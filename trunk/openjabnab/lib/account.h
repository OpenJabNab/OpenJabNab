#ifndef _ACCOUNT_H_
#define _ACCOUNT_H_

#include <QByteArray>
#include <QObject>
#include <QString>
#include "apimanager.h"
#include "global.h"

class OJN_EXPORT Account : QObject
{
	Q_OBJECT
public:
	Account(QString const& login);
	Account(QString const& login, QByteArray const& password, QString const& name, QByteArray const& id);
	~Account();

	void SetBunny(QString const& name, QByteArray const& id) { bunnyName = name; bunnyID = id; }
	void SetPasswordHash(QByteArray const& password) { passwordHash = password; }

	QString GetLoginName() { return accountLogin; }
	QString GetBunnyName() { return bunnyName; }
	QByteArray GetBunnyID() { return bunnyID; }
	bool IsGoodPassword(QByteArray const& password) { return (bool)(passwordHash == password); }

private slots:
	void SaveAccount();
	
private:
	void LoadAccount();
	void InitAccountFile();

	QString accountLogin;
	QString bunnyName;
	QByteArray bunnyID;
	QByteArray passwordHash;

	QString accountFileName;
};
#endif
