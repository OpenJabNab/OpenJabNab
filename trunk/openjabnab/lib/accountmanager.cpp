#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
#include <QLibrary>
#include <QString>
#include "account.h"
#include "accountmanager.h"
#include "apimanager.h"
#include "log.h"
#include "httprequest.h"

AccountManager::AccountManager()
{
	QDir accountsDir = QCoreApplication::applicationDirPath();
	accountsDir.cd("accounts");

	Log::Info(QString("Finding accounts in : %1").arg(accountsDir.path()));
	
	foreach (QString fileName, accountsDir.entryList(QDir::Files)) 
	{
		QFile file(accountsDir.absoluteFilePath(fileName));
		if (!file.open(QIODevice::ReadOnly))
		{
			Log::Error("Cannot open account file for reading : " + fileName);
		}
		QString status = QString(" - %1 : ").arg(fileName);
		QString accountLogin;
		QDataStream in(&file);
		in.setVersion(QDataStream::Qt_4_3);
		in >> accountLogin;
		if (in.status() == QDataStream::Ok)
		{
			Account * a = new Account(accountLogin);
			listOfAccounts.append(a);
			listOfAccountsByName.insert(a->GetLoginName(), a);
			status.append(a->GetLoginName() + " (Bunny: "+a->GetBunnyName()+") OK" );
		}
		else
			status.append(" Failed, "+ QString::number(in.status())); 
		Log::Info(status);
	}
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

QByteArray AccountManager::Login(QString const& login, QByteArray const& hash)
{
	if(!listOfAccountsByName.contains(login))
		return "BAD_LOGIN";
	Account * a = GetAccountByName(login);
	if(a->IsGoodPassword(hash))
		return GenerateNewToken(a);
	return "BAD_PASSWORD";
}

QByteArray AccountManager::GenerateNewToken(Account * a)
{
	QByteArray token = QCryptographicHash::hash((a->GetLoginName()+QDateTime::currentDateTime().toString(Qt::ISODate)).toAscii(), QCryptographicHash::Md5).toHex();
	listOfToken.insert(token, a);
	listOfTokenExpiration.insert(token, QDateTime::currentDateTime().addSecs(1800));
	return token;
}

bool AccountManager::checkTokenValidity(QByteArray token, Account * a)
{
	Account	* b = listOfToken.value(token);
	if(a != b)
		return false;
	if(listOfTokenExpiration.value(token) < QDateTime::currentDateTime())
		return false;
	return true;

}

void AccountManager::updateToken(QByteArray token)
{
	listOfTokenExpiration[token] = QDateTime::currentDateTime().addSecs(1800);
}

ApiManager::ApiAnswer * AccountManager::ProcessApiCall(QByteArray const& request, HTTPRequest const& hRequest)
{
	if(request.startsWith("getListOfAccounts"))
	{
		QList<QByteArray> list;
		foreach (Account * a, listOfAccounts)
			list.append(a->GetLoginName().toAscii());

		return new ApiManager::ApiList(list);
	}
	else if(request.startsWith("loginAccount"))
	{
		if(!hRequest.HasArg("login") || !hRequest.HasArg("hash"))
			return new ApiManager::ApiError("Missing arguments<br />Request was : " + hRequest.toString());

		QByteArray retour = Login(hRequest.GetArg("login"), hRequest.GetArg("hash").toAscii());
		if(retour == "BAD_LOGIN")
			return new ApiManager::ApiError("Login '"+hRequest.GetArg("login")+"' doesn't exist");

		if(retour == "")
			return new ApiManager::ApiError("Bad password for login '"+hRequest.GetArg("login")+"'");

		return new ApiManager::ApiString(retour);
			
	}
	else if(request.startsWith("getAccountInfo"))
	{
		if(!hRequest.HasArg("login"))
			return new ApiManager::ApiError("Missing argument 'login'<br />Request was : " + hRequest.toString());

		if(!listOfAccountsByName.contains(hRequest.GetArg("login")))
			return new ApiManager::ApiError("Account '"+hRequest.GetArg("login")+"' doesn't exists");
			
		Account * a = GetAccountByName(hRequest.GetArg("login"));
		QMap<QByteArray, QByteArray> list;
		list.insert("Login", a->GetLoginName().toAscii());
		list.insert("Bunny", a->GetBunnyName().toAscii());
		list.insert("Serial", a->GetBunnyID());
		list.insert("Admin", "true");
		return new ApiManager::ApiMappedList(list);
	}
	else if(request.startsWith("registerNewAccount"))
	{
		if(!hRequest.HasArg("login") || !hRequest.HasArg("hash") || !hRequest.HasArg("name") || !hRequest.HasArg("bunny"))
			return new ApiManager::ApiError("Missing arguments<br />Request was : " + hRequest.toString());

		if(listOfAccountsByName.contains(hRequest.GetArg("login")))
			return new ApiManager::ApiError("Account '"+hRequest.GetArg("login")+"' already exists");
			
		Account * a = new Account(hRequest.GetArg("login"), hRequest.GetArg("hash").toAscii(), hRequest.GetArg("name"), hRequest.GetArg("bunny").toAscii());
		listOfAccounts.append(a);
		listOfAccountsByName.insert(a->GetLoginName(), a);
		return new ApiManager::ApiString("New account created : "+hRequest.GetArg("login"));
	}
	else
		return new ApiManager::ApiError("Unknown Accounts Api Call : " + request + "<br />Request was : " + hRequest.toString());
}
