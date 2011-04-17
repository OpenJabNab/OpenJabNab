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
			GlobalAccess = Read;
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
		in >> login >> username >> passwordHash >> isAdmin >> PluginsAccess >> BunniesAccess >> GlobalAccess >> GeneralPluginAccess >> listOfBunnies;
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
}

void Account::SetDefault()
{
	// By default NO ACCESS
	isAdmin = false;
	PluginsAccess = None;
	BunniesAccess = None;
	GlobalAccess = None;
	GeneralPluginAccess = None;
}

QDataStream & operator<< (QDataStream & out, const Account & a)
{
	out << a.login << a.username << a.passwordHash << a.isAdmin << a.PluginsAccess << a.BunniesAccess << a.GlobalAccess << a.GeneralPluginAccess << a.listOfBunnies;
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


void Account::InitApiCalls()
{
	DECLARE_API_CALL("info()", &Account::Api_Info);
}

API_CALL(Account::Api_Info)
{
	Q_UNUSED(hRequest);

	if(!account.IsAdmin())
		return new ApiManager::ApiError("Access denied");

	QMap<QString, QString> list;
	list.insert("login", account.GetLogin());
	list.insert("token", account.GetToken());
	list.insert("admin", account.IsAdmin() ? "true" : "false" );

	return new ApiManager::ApiMappedList(list);
}

