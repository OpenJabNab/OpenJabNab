#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QLibrary>
#include <QString>
#include <QUuid>
#include "account.h"
#include "accountmanager.h"
#include "apimanager.h"
#include "bunny.h"
#include "bunnymanager.h"
#include "ztamp.h"
#include "bunnymanager.h"
#include "log.h"
#include "httprequest.h"
#include "settings.h"

AccountManager::AccountManager()
{
	accountsDir = QCoreApplication::applicationDirPath();
	if (!accountsDir.cd("accounts"))
	{
		if (!accountsDir.mkdir("accounts"))
		{
			LogError("Unable to create accounts directory !\n");
			exit(-1);
		}
		accountsDir.cd("accounts");
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

void AccountManager::LoadAccounts()
{
	LogInfo(QString("Finding accounts in : %1").arg(accountsDir.path()));
	/* Apply filters on accounts files */
	QStringList filters;
	filters << "*.dat";
	accountsDir.setNameFilters(filters);
	foreach (QFileInfo ffile, accountsDir.entryInfoList(QDir::Files))
	{
		/* Open File */
		QByteArray configFileName = accountsDir.absoluteFilePath(ffile.fileName().toAscii()).toAscii();
		QFile file(configFileName);
		if (!file.open(QIODevice::ReadOnly))
		{
			LogError(QString("Cannot open config file for reading : %1").arg(QString(configFileName)));
			continue;
		}
		QDataStream in(&file);
		in.setVersion(QDataStream::Qt_4_3);
		int version;
		in >> version;
		Account * a = new Account(in, version);
		if (in.status() != QDataStream::Ok)
		{
			LogWarning(QString("Problem when loading config file for account: %1").arg(QString(configFileName)));
			delete a;
			continue;
		}
		listOfAccounts.append(a);
		listOfAccountsByName.insert(a->GetLogin(), a);
	}

	/* Bound to disappear on next releases */
	QDir appDir(QCoreApplication::applicationDirPath());
	if(listOfAccounts.count() == 0 && appDir.exists("accounts.dat"))
	{
		LogWarning("Using Old Config File for accounts");
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
	/* Old Config File End */

	if(listOfAccounts.count() == 0)
	{
		LogWarning("No account loaded ... inserting default admin");
		Account * a = new Account(Account::DefaultAdmin);
		listOfAccounts.append(new Account(Account::DefaultAdmin));
		listOfAccountsByName.insert(a->GetLogin(), a);
	}
	LogInfo(QString("Total of accounts: %1").arg(listOfAccounts.count()));
}

void AccountManager::SaveAccounts()
{
	/* For each loaded account */
	foreach(Account * a, listOfAccounts) {
			/* Skip default admin */
			if(a->GetLogin() != "admin") {
				/* Select file */
				QFile accountFile(accountsDir.absoluteFilePath(QString("%1.dat").arg(a->GetLogin())));
				/* Open it, write access */
				if(accountFile.open(QIODevice::WriteOnly))
				{
					/* Save Version */
					QDataStream out(&accountFile);
					out.setVersion(QDataStream::Qt_4_3);
					out << Account::Version();
					/* Save Data */
					out << *a;
				}	else
					LogError(QString("Can't open %1.dat, account will not be saved").arg(a->GetLogin()));
		}
	}
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
			it->expire_time = now + GlobalSettings::GetInt("Config/SessionTimeout", 300); // default : 5min
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

Account * AccountManager::GetAccountByLogin(QByteArray const& login)
{
	if(Instance().listOfAccountsByName.contains(login))
		return Instance().listOfAccountsByName.value(login);
	return NULL;
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
			t.expire_time = QDateTime::currentDateTime().toTime_t() + GlobalSettings::GetInt("Config/SessionTimeout", 300);
			(*it)->SetToken(token);
			listOfTokens.insert(token, t);
			return token;
		}
		LogError(QString("Bad login : user=%1, hash=%2, proposed hash=%3").arg(login,QString((*it)->GetPasswordHash().toHex()),QString(hash.toHex())));
		return QByteArray();
	}
	LogError(QString("Bad login : user=%1").arg(QString(login)));
	return QByteArray();
}

/*******
 * API *
 *******/

void AccountManager::InitApiCalls()
{
	DECLARE_API_CALL("auth(login,pass)", &AccountManager::Api_Auth);
	DECLARE_API_CALL("changePassword(login,pass)", &AccountManager::Api_ChangePasswd);
	DECLARE_API_CALL("registerNewAccount(login,username,pass)", &AccountManager::Api_RegisterNewAccount);
	DECLARE_API_CALL("addBunny(login,bunnyid)", &AccountManager::Api_AddBunny);
	DECLARE_API_CALL("removeBunny(login,bunnyid)", &AccountManager::Api_RemoveBunny);
	DECLARE_API_CALL("removeZtamp(login,zid)", &AccountManager::Api_RemoveZtamp);
	DECLARE_API_CALL("settoken(tk)", &AccountManager::Api_SetToken);
	DECLARE_API_CALL("setadmin(user)", &AccountManager::Api_SetAdmin);
	DECLARE_API_CALL("infos(user)", &AccountManager::Api_GetUserInfos);
	DECLARE_API_CALL("GetUserlist()", &AccountManager::Api_GetUserlist);
	DECLARE_API_CALL("GetConnectedUsers()", &AccountManager::Api_GetConnectedUsers);
	DECLARE_API_CALL("GetListOfAdmins()", &AccountManager::Api_GetListOfAdmins);
}

API_CALL(AccountManager::Api_Auth)
{
	Q_UNUSED(account);

	QByteArray retour = GetToken(hRequest.GetArg("login"), QCryptographicHash::hash(hRequest.GetArg("pass").toAscii(), QCryptographicHash::Md5));
	if(retour == QByteArray())
		return new ApiManager::ApiError("Access denied");

	LogInfo(QString("User login : %1").arg(hRequest.GetArg("login")));
	return new ApiManager::ApiString(retour);
}

API_CALL(AccountManager::Api_ChangePasswd)
{
	QString login = hRequest.GetArg("login");
	QString pwd = hRequest.GetArg("pass");
	LogWarning(QString("Login: %1 Pwd: %2 user %3").arg(login,pwd,account.GetLogin()));
	if(login == "" || pwd == "" || (!account.IsAdmin() && login != account.GetLogin()))
		return new ApiManager::ApiError("Access denied");

	Account *ac = listOfAccountsByName.value(login.toAscii());
	if(ac == NULL)
		return new ApiManager::ApiError("Login not found.");

	ac->SetPassword(QCryptographicHash::hash(pwd.toAscii(), QCryptographicHash::Md5));
	LogInfo(QString("Password changed for user '%1'").arg(login));
	SaveAccounts();
	return new ApiManager::ApiOk("Password changed");
}

API_CALL(AccountManager::Api_RegisterNewAccount)
{
	if(GlobalSettings::Get("Config/AllowAnonymousRegistration", false) == false && !account.IsAdmin())
		return new ApiManager::ApiError("Access denied");

	QString login = hRequest.GetArg("login");
	if(listOfAccountsByName.contains(login))
		return new ApiManager::ApiError(QString("Account '%1' already exists").arg(hRequest.GetArg("login")));

	Account * a = new Account(login, hRequest.GetArg("username"), QCryptographicHash::hash(hRequest.GetArg("pass").toAscii(), QCryptographicHash::Md5));
	listOfAccounts.append(a);
	listOfAccountsByName.insert(a->GetLogin(), a);
	if(listOfAccounts.count() == 2 && listOfAccountsByName.contains("admin")) {
		LogWarning("Registering first account, set him admin.");
		a->setAdmin();
		//Todo: Drop default admin right now, security issues
	}
	SaveAccounts();
	return new ApiManager::ApiOk(QString("New account created : %1").arg(hRequest.GetArg("login")));
}

API_CALL(AccountManager::Api_AddBunny)
{
	// Only admins can add a bunny to an account
	if(GlobalSettings::Get("Config/AllowUserManageBunny", false) == false && !account.IsAdmin())
		return new ApiManager::ApiError("Access denied");

	QString login = hRequest.GetArg("login");
	if(!account.IsAdmin() && login != account.GetLogin())
		return new ApiManager::ApiError("Access denied");

	if(!listOfAccountsByName.contains(login))
		return new ApiManager::ApiError(QString("Account '%1' doesn't exist").arg(hRequest.GetArg("login")));
	QString bunnyid = hRequest.GetArg("bunnyid");

	// Lock bunny to this account
	Bunny *b = BunnyManager::GetBunny(bunnyid.toAscii());
	QString own = b->GetGlobalSetting("OwnerAccount","").toString();
	if(own != "" && own != login)
		return new ApiManager::ApiError(QString("Bunny %1 is already attached to this account: '%2'").arg(bunnyid,own));

	b->SetGlobalSetting("OwnerAccount", login);
	QByteArray id = listOfAccountsByName.value(login)->AddBunny(bunnyid.toAscii());
	SaveAccounts();
	return new ApiManager::ApiOk(QString("Bunny '%1' added to account '%2'").arg(QString(id)).arg(login));
}

API_CALL(AccountManager::Api_RemoveBunny)
{
	// Only admin can remove bunny to any accounts, else an auth user can remove a bunny from his account
	QString login = hRequest.GetArg("login");
	/* Account doesn't exist */
	if(!listOfAccountsByName.contains(login))
			return new ApiManager::ApiError(QString("Account '%1' doesn't exist").arg(login));
	/* user is not admin and (is not allowed or it's not his account) */
	else if(!account.IsAdmin() && (GlobalSettings::Get("Config/AllowUserManageBunny", false) != true || account.GetLogin() != login))
			return new ApiManager::ApiError(QString("Access denied to user '%1'").arg(login));

	QString bunnyID = hRequest.GetArg("bunnyid");
	if(listOfAccountsByName.value(login)->RemoveBunny(bunnyID.toAscii())) {
		Bunny *b = BunnyManager::GetBunny(bunnyID.toAscii());
		b->RemoveGlobalSetting("OwnerAccount");
		SaveAccounts();
		return new ApiManager::ApiOk(QString("Bunny '%1' removed from account '%2'").arg(bunnyID).arg(login));
	} else
		return new ApiManager::ApiError(QString("Can't remove bunny '%1' from account '%2'").arg(bunnyID).arg(login));
}

API_CALL(AccountManager::Api_RemoveZtamp)
{
	// Only admin can remove ztamp to any accounts, else an auth user can remove a ztamp from his account
	QString login = hRequest.GetArg("login");
	/* Account doesn't exist */
	if(!listOfAccountsByName.contains(login))
			return new ApiManager::ApiError(QString("Account '%1' doesn't exist").arg(login));
	/* user is not admin and (is not allowed or it's not his account) */
	else if(!account.IsAdmin() && (GlobalSettings::Get("Config/AllowUserManageZtamp", false) != true || account.GetLogin() != login))
			return new ApiManager::ApiError(QString("Access denied to user '%1'").arg(login));

	QString zID = hRequest.GetArg("zid");
	if(listOfAccountsByName.value(login)->RemoveZtamp(zID.toAscii())) {
		Ztamp *z = ZtampManager::GetZtamp(zID.toAscii());
		z->RemoveGlobalSetting("OwnerAccount");
		SaveAccounts();
		return new ApiManager::ApiOk(QString("Ztamp '%1' removed from account '%2'").arg(zID).arg(login));
	} else
		return new ApiManager::ApiError(QString("Can't remove ztamp '%1' from account '%2'").arg(zID).arg(login));
}

API_CALL(AccountManager::Api_SetToken)
{
	QHash<QString, Account *>::iterator it = listOfAccountsByName.find(account.GetLogin());
	if(it != listOfAccountsByName.end())
	{
		it.value()->SetToken(hRequest.GetArg("tk").toAscii());
		//SaveAccounts();
		return new ApiManager::ApiString("Token changed");
	}

	//LogError("Account not found");
	return new ApiManager::ApiError("Access denied");
}

API_CALL(AccountManager::Api_GetUserInfos)
{
	QString login = hRequest.GetArg("user");
	if(login == "" || (!account.IsAdmin() && login != account.GetLogin()))
		return new ApiManager::ApiError("Access denied");

	Account *ac = listOfAccountsByName.value(login.toAscii());
	if(ac == NULL)
		return new ApiManager::ApiError("Login not found.");

	QMap<QString, QVariant> list;
	list.insert("login",ac->GetLogin());
	list.insert("username",ac->GetUsername());
	list.insert("isValid",listOfTokens.contains(ac->GetToken()));
	list.insert("token",QString(ac->GetToken()));
	list.insert("isAdmin",ac->IsAdmin());
	return new ApiManager::ApiMappedList(list);
}

API_CALL(AccountManager::Api_GetUserlist)
{
	Q_UNUSED(hRequest);
	if(!account.IsAdmin())
		return new ApiManager::ApiError("Access denied");

	QMap<QString, QVariant> list;
	foreach (Account* a, listOfAccounts)
		list.insert(a->GetLogin(),a->GetUsername());

	return new ApiManager::ApiMappedList(list);
}

API_CALL(AccountManager::Api_GetConnectedUsers)
{
	Q_UNUSED(hRequest);
	if(!account.IsAdmin())
		return new ApiManager::ApiError("Access denied");

	QList<QString> list;
	foreach (Account* a, listOfAccounts)
		if(listOfTokens.contains(a->GetToken()))
			list.append(a->GetLogin());
	return new ApiManager::ApiList(list);
}

API_CALL(AccountManager::Api_GetListOfAdmins)
{
	Q_UNUSED(hRequest);
	if(!account.IsAdmin())
		return new ApiManager::ApiError("Access denied");

	QList<QString> list;
	foreach (Account* a, listOfAccounts)
		if(a->IsAdmin())
			list.append(a->GetLogin());
	return new ApiManager::ApiList(list);
}

API_CALL(AccountManager::Api_SetAdmin)
{
	QString login = hRequest.GetArg("user");

	if(login == "" || !account.IsAdmin())
		return new ApiManager::ApiError("Access denied");

	/* Get User */
	Account *ac = listOfAccountsByName.value(login.toAscii());
	if(ac == NULL)
		return new ApiManager::ApiError("Login not found.");
	ac->setAdmin();
	return new ApiManager::ApiOk(QString("user '%1' is now admin").arg(login));
}
