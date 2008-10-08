#ifndef _LOG_H_
#define _LOG_H_

#include <QString>
#include <QTextStream>
#include "global.h"

class QFile;
class OJN_EXPORT Log
{
private:
	enum LogLevel { Log_None = 0, Log_Info, Log_Error, Log_Warn, Log_Debug};

public:
	static void Instantiate() { if(!instance) instance = new Log(); }
	static void Release() { if (instance) { delete instance; } }
	~Log();
	static void LogToFile(QString const&, LogLevel);
	static void Debug(QString const& data) { LogToFile(data, Log_Debug); }
	static void Warning(QString const& data) { LogToFile(data, Log_Warn); }
	static void Error(QString const& data) { LogToFile(data, Log_Error); }
	static void Info(QString const& data) { LogToFile(data, Log_Info); }
	
private:
	Log();
	LogLevel GetLevel(QString const&);
	
	static Log * instance;
	QTextStream logStream;
	LogLevel maxFileLogLevel;
	LogLevel maxScreenLogLevel;
	QFile * logFile;
};

#endif
