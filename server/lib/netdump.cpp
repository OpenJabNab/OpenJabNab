#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <iostream>
#include "log.h"
#include "netdump.h"
#include "settings.h"

void NetworkDump::Init()
{
	// Open dump file
	QFile * dumpFile = new QFile(QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("dump.log"));
	if(!dumpFile->open(QIODevice::Append))
	{
		LogError(QString("Error opening file : %1").arg(dumpFile->fileName()));
		return;
	}
	Instance().dumpStream.setDevice(dumpFile);
	Instance().dumpStream << QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss") << " -- OpenJabNab Start --" << endl;
}

void NetworkDump::Close()
{
	Instance().dumpStream << QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss") << " -- OpenJabNab End --" << endl;
	QIODevice * d = Instance().dumpStream.device();
	if (d)
	{
		d->close();
		delete d;
	}
}

void NetworkDump::Log(QString const& what, QString const& txt)
{
	Instance().dumpStream << QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss") << " - " << what << " - " << txt << endl;
}

NetworkDump & NetworkDump::Instance()
{
	static NetworkDump n;
	return n;
}
