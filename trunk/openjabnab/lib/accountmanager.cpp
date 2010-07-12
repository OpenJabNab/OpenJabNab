#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
#include <QLibrary>
#include <QString>
#include <QUuid>
#include "account.h"
#include "accountmanager.h"
#include "apimanager.h"
#include "log.h"
#include "httprequest.h"

AccountManager::AccountManager()
{
}

AccountManager & AccountManager::Instance()
{
  static AccountManager p;
  return p;
}

AccountManager::~AccountManager()
{
	foreach(Account * a, listOfAccounts)
		delete a;
}

void AccountManager::InitApiCalls()
{
	DECLARE_API_CALL("auth", &AccountManager::Api_Auth);
	DECLARE_API_CALL("registerNewAccount", &AccountManager::Api_RegisterNewAccount);
	DECLARE_API_CALL("addBunny", &AccountManager::Api_AddBunny);
	DECLARE_API_CALL("removeBunny", &AccountManager::Api_RemoveBunny);
}

void AccountManager::LoadAccounts()
{
	QDir appDir(QCoreApplication::applicationDirPath());
	if(appDir.exists("accounts.dat"))
	{
		QFile accountsFile(appDir.absoluteFilePath("accounts.dat"));
		if(accountsFile.open(QIODevice::ReadOnly))
		{
			QDataStream in(&accountsFile);
			in.setVersion(QDataStream::Qt_4_3);
			int version;
			in >> version;
			while(!in.atEnd())
			{
				Account * a = new Account(in, version);
				if(in.status() == QDataStream::Ok)
				{
					listOfAccounts.append(a);
					listOfAccountsByName.insert(a->GetLogin(), a);
				}
				else
				{
					LogError("Bad account file, stop parsing");
					delete a;
					break;
				}
			}
		}
		else
			LogError("Can't open accounts.dat");
	}
	if(listOfAccounts.count() == 0)
	{
		LogWarning("No account loaded ... inserting default admin");
		Account * a = new Account(Account::DefaultAdmin);
		listOfAccounts.append(new Account(Account::DefaultAdmin));
		listOfAccountsByName.insert(a->GetLogin(), a);
	}
}

void AccountManager::SaveAccounts()
{
	QDir appDir(QCoreApplication::applicationDirPath());
	QFile accountsFile(appDir.absoluteFilePath("accounts.dat"));
	if(accountsFile.open(QIODevice::WriteOnly))
	{
		QDataStream out(&accountsFile);
		out.setVersion(QDataStream::Qt_4_3);
		out << Account::Version();
		foreach(Account * a, listOfAccounts)
			out << *a;
	}
	else
		LogError("Can't open accounts.dat, accounts will not be saved");
}

Account const& AccountManager::Guest()
{
	static Account guest(Account::Guest);
	return guest;
}

Account const& AccountManager::GetAccount(QByteArray const& token)
{
	QHash<QByteArray, TokenData>::iterator it = listOfTokens.find(token);
	if(it != listOfTokens.end())
	{
		unsigned int now = QDateTime::currentDateTime().toTime_t();
		if(now < it->expire_time)
		{
			it->expire_time = now + 300; // 5min
			return *(it->account);
		}
		else
		{
			listOfTokens.erase(it);
			return Guest();
		}
	}
	return Guest();
}

QByteArray AccountManager::GetToken(QString const& login, QByteArray const& hash)
{
	QHash<QString, Account *>::const_iterator it = listOfAccountsByName.find(login);
	if(it != listOfAccountsByName.end())
	{
		if((*it)->GetPasswordHash() == hash)
		{
			// Generate random token
			QByteArray token = QCryptographicHash::hash(QUuid::createUuid().toString().toAscii(), QCryptographicHash::Md5).toHex();
			TokenData t;
			t.account = *it;
			t.expire_time = QDateTime::currentDateTime().toTime_t() + 300;
			listOfTokens.insert(token, t);
			return token;
		}
		LogError(QString("Bad login : user=%1, hash=%2, proposed hash=%3").arg(login,QString((*it)->GetPasswordHash().toHex()),QString(hash.toHex())));
		return QByteArray();
	}
	LogError(QString("Bad login : user=%1").arg(QString(login)));
	return QByteArray();
}

// API
API_CALL(AccountManager::Api_Auth)
{
	Q_UNUSED(account);

	if(!hRequest.HasArg("login") || !hRequest.HasArg("pass"))
		return new ApiManager::ApiError(QString("Missing arguments<br />Request was : %1").arg(hRequest.toString()));

	QByteArray retour = GetToken(hRequest.GetArg("login"), QCryptographicHash::hash(hRequest.GetArg("pass").toAscii(), QCryptographicHash::Md5));
	if(retour == QByteArray())
		return new ApiManager::ApiError("Access denied");

	LogInfo(QString("User login : %1").arg(hRequest.GetArg("login")));
	return new ApiManager::ApiString(retour);
}

API_CALL(AccountManager::Api_RegisterNewAccount)
{
	if(!account.IsAdmin())
		return new ApiManager::ApiError("Access denied");

	if(!hRequest.HasArg("login") || !hRequest.HasArg("username") || !hRequest.HasArg("pass"))
		return new ApiManager::ApiError(QString("Missing arguments<br />Request was : %1").arg(hRequest.toString()));

	QString login = hRequest.GetArg("login");
	if(listOfAccountsByName.contains(login))
		return new ApiManager::ApiError(QString("Account '%1' already exists").arg(hRequest.GetArg("login")));
		
	Account * a = new Account(login, hRequest.GetArg("username"), QCryptographicHash::hash(hRequest.GetArg("pass").toAscii(), QCryptographicHash::Md5));
	listOfAccounts.append(a);
	listOfAccountsByName.insert(a->GetLogin(), a);
	return new ApiManager::ApiOk(QString("New account created : %1").arg(hRequest.GetArg("login")));
}

API_CALL(AccountManager::Api_AddBunny)
{
	// Only admins can add a bunny to an account
	if(!account.IsAdmin())
		return new ApiManager::ApiError("Access denied");

	if(!hRequest.HasArg("login") || !hRequest.HasArg("bunnyid"))
		return new ApiManager::ApiError(QString("Missing arguments<br />Request was : %1").arg(hRequest.toString()));

	QString login = hRequest.GetArg("login");
	if(!listOfAccountsByName.contains(login))
		return new ApiManager::ApiError(QString("Account '%1' doesn't exist").arg(hRequest.GetArg("login")));

	QByteArray id = listOfAccountsByName.value(login)->AddBunny(hRequest.GetArg("bunnyid").toAscii());
	return new ApiManager::ApiOk(QString("Bunny '%1' added to account '%2'").arg(QString(id)).arg(login));
}

API_CALL(AccountManager::Api_RemoveBunny)
{
	if(!hRequest.HasArg("login") || !hRequest.HasArg("bunnyid"))
		return new ApiManager::ApiError(QString("Missing arguments<br />Request was : %1").arg(hRequest.toString()));

	// Only admin can remove bunny to any accounts, else an auth user can remove a bunny from his account
	QString login = hRequest.GetArg("login");
	if(account.IsAdmin() && !listOfAccountsByName.contains(login))
			return new ApiManager::ApiError(QString("Account '%1' doesn't exist").arg(hRequest.GetArg("login")));
	else if(account.GetLogin() != login)
			return new ApiManager::ApiError(QString("Access denied to user '%1'").arg(login));

	QString bunnyID = hRequest.GetArg("bunnyid");
	if(listOfAccountsByName.value(login)->RemoveBunny(bunnyID.toAscii()))
		return new ApiManager::ApiOk(QString("Bunny '%1' removed from account '%2'").arg(bunnyID).arg(login));
	else
		return new ApiManager::ApiError(QString("Can't remove bunny '%1' from to account '%2'").arg(bunnyID).arg(login));
}
