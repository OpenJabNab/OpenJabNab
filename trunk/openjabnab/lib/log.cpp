#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <iostream>
#include "log.h"
#include "settings.h"

Log::Log():maxFileLogLevel(Log_Debug),maxScreenLogLevel(Log_Warn)
{
	if (!GlobalSettings::HasKey("Log/LogFile"))
	{
		std::cerr << "Log/LogFile not set in configuration file ! Logging only on screen !" << std::endl;
		maxFileLogLevel = Log_None;
		return;
	}

	QFile * logFile = new QFile(QDir(QCoreApplication::applicationDirPath()).absoluteFilePath(GlobalSettings::GetString("Log/LogFile")));
	if(!logFile->open(QIODevice::Append))
	{
		std::cerr << "Error opening file : " << qPrintable(logFile->fileName()) << std::endl << "Logging only on screen !" << std::endl;
		maxFileLogLevel = Log_None;
	}
	else
	{
		logStream.setDevice(logFile);
		logStream << QDateTime::currentDateTime().toString("[dd/MM/yyyy hh:mm:ss] ") << "-- OpenJabNab Start --" << endl;
	}
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
