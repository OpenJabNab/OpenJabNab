#include <QCryptographicHash>
#include <QCoreApplication>
#include <QDataStream>
#include <QFlag>
#include "account.h"
#include "log.h"

Account::Account()
{
	SetDefault();
}

Account::Account(SpecialAccount t)
{
	switch(t)
	{
		case Guest:
			SetDefault(); // Default values
			login = "guest";
			username = "Guest";
			break;

		case DefaultAdmin:
			login = "admin";
			username = "Administrator";
			passwordHash = QCryptographicHash::hash("admin", QCryptographicHash::Md5);
			isAdmin = true;
			break;
	}
}

Account::Account(QDataStream & in, unsigned int version)
{
	SetDefault();
	if(version == 1)
	{
		in >> login >> username >> passwordHash >> isAdmin >> UserAccess >> listOfBunnies >> listOfZtamps;
	}
	else
		LogError(QString("Can't load account with version %1").arg(version));
}

Account::Account(QString const& l, QString const& u, QByteArray const& p)
{
	SetDefault();
	login = l;
	username = u;
	passwordHash = p;
	UserAccess[AcGlobal] = Read;
	UserAccess[AcAccount] = ReadWrite;
	UserAccess[AcBunnies] = ReadWrite;
	UserAccess[AcZtamps] = ReadWrite;
	UserAccess[AcPlugins] = Read;
	UserAccess[AcPluginsBunny] = ReadWrite;
	UserAccess[AcPluginsZtamp] = ReadWrite;
}

void Account::SetDefault()
{
	// By default NO ACCESS
	isAdmin = false;
	UserAccess.insert(AcGlobal,None);
	UserAccess.insert(AcAccount,None);
	UserAccess.insert(AcBunnies,None);
	UserAccess.insert(AcZtamps,None);
	UserAccess.insert(AcPluginsBunny,None);
	UserAccess.insert(AcPluginsZtamp,None);
	UserAccess.insert(AcPlugins,None);
	UserAccess.insert(AcServer,None);
}

QDataStream & operator<< (QDataStream & out, const Account & a)
{
	out << a.login << a.username << a.passwordHash << a.isAdmin << a.UserAccess << a.listOfBunnies << a.listOfZtamps;
	return out;
}

QDataStream & operator>> (QDataStream & in, Account::Rights & r)
{
	int value;
	in >> value;
	r = QFlag(value);
	return in;
}

QDataStream & operator<< (QDataStream & out, const Account::Rights & r)
{
	out << (int)r;
	return out;
}
