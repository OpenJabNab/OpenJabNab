#include <QCoreApplication>
#include <QTimer>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <iostream>
#include "log.h"
#include "settings.h"

Log::Log()
{
	if (!GlobalSettings::HasKey("Log/LogFile"))
	{
		std::cerr << "Log/LogFile not set in configuration file ! Logging only on screen !" << std::endl;
		maxFileLogLevel = Log_None;
		return;
	}

	maxFileLogLevel = GetLevel(GlobalSettings::GetString("Log/LogFileLevel", "Debug"));
	maxScreenLogLevel = GetLevel(GlobalSettings::GetString("Log/LogScreenLevel", "Warning"));
	
	logFile = new QFile(QDir(QCoreApplication::applicationDirPath()).absoluteFilePath(GlobalSettings::GetString("Log/LogFile")));
	if(!logFile->open(QIODevice::Append))
	{
		std::cerr << "Error opening file : " << qPrintable(logFile->fileName()) << std::endl << "Logging only on screen !" << std::endl;
		maxFileLogLevel = Log_None;
	}
	else
		logStream.setDevice(logFile);
}

void Log::LogToFile(QString const& data, LogLevel level)
{
	Log::LogToFile(data, level, false);
}

void Log::LogToFile(QString const& data, LogLevel level, bool rotate)
{	
	static Log instance;

	if(rotate)
	{
		QString fileName = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath(GlobalSettings::GetString("Log/LogFile"));

		QString archiveName = fileName;
		archiveName.replace(".log", "." + QDateTime::currentDateTime().addDays(-1).toString("yyyyMMdd") + ".log");
		instance.logFile->rename(archiveName);

		instance.logFile = NULL;
		delete instance.logFile;
		instance.logFile = new QFile(QDir(QCoreApplication::applicationDirPath()).absoluteFilePath(GlobalSettings::GetString("Log/LogFile")));
        	if(!instance.logFile->open(QIODevice::Append))
        	{
        	        std::cerr << "Error opening file : " << qPrintable(instance.logFile->fileName()) << std::endl << "Logging only on screen !" << std::endl;
        	        instance.maxFileLogLevel = Log_None;
        	}
        	else
		{
			instance.logStream.flush();
        	        instance.logStream.setDevice(instance.logFile);
		}
	}

	if (level <= instance.maxFileLogLevel)
		instance.logStream << QDateTime::currentDateTime().toString("[dd/MM/yyyy hh:mm:ss] ") << data << endl;

	if (level <= instance.maxScreenLogLevel)
		std::cout << qPrintable(data) << std::endl;
}

Log::LogLevel Log::GetLevel(QString const& level)
{
	if (level.compare("debug", Qt::CaseInsensitive) == 0)
		return Log_Debug;
	if (level.compare("warning", Qt::CaseInsensitive) == 0)
		return Log_Warn;
	if (level.compare("error", Qt::CaseInsensitive) == 0)
		return Log_Error;
	if (level.compare("info", Qt::CaseInsensitive) == 0)
		return Log_Info;
	return Log_None;
}
