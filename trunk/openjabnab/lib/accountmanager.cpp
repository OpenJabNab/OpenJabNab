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

void AccountManager::LoadAccounts()
{
	QDir appDir(QCoreApplication::applicationDirPath());
	if(appDir.exists("accounts.dat"))
	{
		QFile accountsFile(appDir.absoluteFilePath("accounts.dat"));
		if(accountsFile.open(QIODevice::ReadOnly))
		{
			QDataStream in(&accountsFile);
			in.setVersion(QDataStream::Qt_4_4);
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
					Log::Error("Bad account file, stop parsing");
					delete a;
					break;
				}
			}
		}
		else
			Log::Error("Can't open accounts.dat");
	}
	if(listOfAccounts.count() == 0)
	{
		Log::Warning("No account loaded ... inserting default admin");
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
		out.setVersion(QDataStream::Qt_4_4);
		out << Account::Version();
		foreach(Account * a, listOfAccounts)
			out << *a;
	}
	else
		Log::Error("Can't open accounts.dat, accounts will not be saved");
}

Account const& AccountManager::Guest()
{
	static Account guest(Account::Guest);
	return guest;
}

Account const& AccountManager::GetAccount(QByteArray const& token)
{
	QMap<QByteArray, TokenData>::iterator it = listOfTokens.find(token);
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

QByteArray AccountManager::GetToken(QByteArray login, QByteArray hash)
{
	QMap<QByteArray, Account *>::const_iterator it = listOfAccountsByName.find(login);
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
		Log::Error(QString("Bad login : user=%1, hash=%2, proposed hash=%3").arg(QString(login)).arg(QString((*it)->GetPasswordHash().toHex())).arg(QString(hash.toHex())));
		return QByteArray();
	}
	Log::Error(QString("Bad login : user=%1").arg(QString(login)));
	return QByteArray();
}

ApiManager::ApiAnswer * AccountManager::ProcessApiCall(Account const& account, QByteArray const& request, HTTPRequest const& hRequest)
{
	if(request == "auth")
	{
		if(!hRequest.HasArg("login") || !hRequest.HasArg("pass"))
			return new ApiManager::ApiError("Missing arguments<br />Request was : " + hRequest.toString());

		QByteArray retour = GetToken(hRequest.GetArg("login").toAscii(), QCryptographicHash::hash(hRequest.GetArg("pass").toAscii(), QCryptographicHash::Md5));
		if(retour == QByteArray())
			return new ApiManager::ApiError(QByteArray("Access denied"));

		return new ApiManager::ApiString(retour);
	}
	else if(request == "registerNewAccount")
	{
		if(!account.IsAdmin())
			return new ApiManager::ApiError(QByteArray("Access denied"));

		if(!hRequest.HasArg("login") || !hRequest.HasArg("username") || !hRequest.HasArg("pass"))
			return new ApiManager::ApiError("Missing arguments<br />Request was : " + hRequest.toString());

		QByteArray login = hRequest.GetArg("login").toAscii();
		if(listOfAccountsByName.contains(login))
			return new ApiManager::ApiError("Account '"+hRequest.GetArg("login")+"' already exists");
			
		Account * a = new Account(login, hRequest.GetArg("username").toAscii(), QCryptographicHash::hash(hRequest.GetArg("pass").toAscii(), QCryptographicHash::Md5));
		listOfAccounts.append(a);
		listOfAccountsByName.insert(a->GetLogin(), a);
		return new ApiManager::ApiOk("New account created : "+hRequest.GetArg("login"));
	}
	else
		return new ApiManager::ApiError("Unknown Accounts Api Call : " + request + "<br />Request was : " + hRequest.toString());
}
