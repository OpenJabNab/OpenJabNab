#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include "account.h"
#include "log.h"

Account::Account(QString const& login)
{
	accountLogin = login;
	InitAccountFile();
	if (QFile::exists(accountFileName))
		LoadAccount();
}

Account::Account(QString const& login, QByteArray const& password, QString const& name, QByteArray const& id)
{
	accountLogin = login;
	passwordHash = password;
	bunnyName = name;
	bunnyID = id;
	InitAccountFile();
	if (!QFile::exists(accountFileName))
		SaveAccount();
}

void Account::InitAccountFile()
{
	QDir accountsDir = QDir(QCoreApplication::applicationDirPath());
	if (!accountsDir.cd("accounts"))
	{
		if (!accountsDir.mkdir("accounts"))
		{
			Log::Error("Unable to create accounts directory !\n");
			exit(-1);
		}
		accountsDir.cd("accounts");
	}
	accountFileName = accountsDir.absoluteFilePath(accountLogin+".dat");
}

Account::~Account()
{
	SaveAccount();
}

void Account::LoadAccount()
{
	QFile file(accountFileName);
	if (!file.open(QIODevice::ReadOnly))
	{
		Log::Error("Cannot open account file for reading : " + accountFileName);
		return;
	}
	QDataStream in(&file);
	in.setVersion(QDataStream::Qt_4_3);
	in >> accountLogin >> passwordHash >> bunnyName >> bunnyID;
	if (in.status() != QDataStream::Ok)
	{
		Log::Warning("Problem when loading account file : " + accountLogin);
	}
}

void Account::SaveAccount()
{
	QFile file(accountFileName);
	if (!file.open(QIODevice::WriteOnly))
	{
		Log::Error("Cannot open account file for writing : " + accountFileName);
		return;
	}
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_4_3);
	out << accountLogin << passwordHash << bunnyName << bunnyID;
}
