#include <QFile>
#include <QCoreApplication>
#include <QDir>
#include <QDateTime>

#include <iostream>

#include "log.h"
#include "settings.h"

Log::Log():maxFileLogLevel(Log_Debug),maxScreenLogLevel(Log_Warn)
{
	QFile * logFile = new QFile(QDir(QCoreApplication::applicationDirPath()).absoluteFilePath(GlobalSettings::GetString("Log/LogFile","openjabnab.log")));
	if(!logFile->open(QIODevice::Append))
	{
		std::cerr << "Error opening file : " << qPrintable(logFile->fileName()) << std::endl;
		return;
	}
	logStream.setDevice(logFile);
	logStream << QDateTime::currentDateTime().toString("[dd/MM/yyyy hh:mm:ss] ") << "-- OpenJabNab Start --" << endl;
}

void Log::LogToFile(QString const& data, LogLevel level)
{	
	if (!instance)
		instance = new Log();

	if (level <= instance->maxFileLogLevel)
		instance->logStream << QDateTime::currentDateTime().toString("[dd/MM/yyyy hh:mm:ss] ") << data << endl;

	if (level <= instance->maxScreenLogLevel)
		std::cout << qPrintable(data) << std::endl;
}

Log * Log::instance = 0;
