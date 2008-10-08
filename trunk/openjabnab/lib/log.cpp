#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
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

Log::~Log()
{
	delete logFile;
}

void Log::LogToFile(QString const& data, LogLevel level)
{	
	if (level <= instance->maxFileLogLevel)
		instance->logStream << QDateTime::currentDateTime().toString("[dd/MM/yyyy hh:mm:ss] ") << data << endl;

	if (level <= instance->maxScreenLogLevel)
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

Log * Log::instance = 0;
