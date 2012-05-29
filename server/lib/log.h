#ifndef _LOG_H_
#define _LOG_H_

#include <QtGlobal>
#include <QString>
#include <QTextStream>
#include "global.h"

class QFile;
class OJN_EXPORT Log
{
public:
	enum LogLevel { Log_None = 0, Log_Info, Log_Error, Log_Warn, Log_Debug};
	static void LogToFile(QString const&, LogLevel, bool);
	static void LogToFile(QString const&, LogLevel);
	static void RotateLog();

private:
	Log();
	LogLevel GetLevel(QString const&);
	
	QTextStream logStream;
	LogLevel maxFileLogLevel;
	LogLevel maxScreenLogLevel;
	QFile * logFile;
};

#define LogRotate() Log::LogToFile("Rotate log", Log::Log_Info, true)
#define LogInfo(data) Log::LogToFile(data, Log::Log_Info)
#define LogError(data) Log::LogToFile(QString("%1 : %2").arg(Q_FUNC_INFO,data), Log::Log_Error)
#define LogWarning(data) Log::LogToFile(QString("%1 : %2").arg(Q_FUNC_INFO,data), Log::Log_Warn)
#define LogDebug(data) Log::LogToFile(QString("%1 : %2").arg(Q_FUNC_INFO,data), Log::Log_Debug)

#endif
