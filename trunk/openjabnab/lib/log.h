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
	static inline void Init() { if(!instance) instance = new Log(); }
	static inline void Close() { if (instance) { delete instance; } }
	~Log();
	static void LogToFile(QString const&, LogLevel);
	static void Debug(QString const& data);
	static void Warning(QString const& data);
	static void Error(QString const& data);
	static void Info(QString const& data);
	
private:
	Log();
	LogLevel GetLevel(QString const&);
	
	static Log * instance;
	QTextStream logStream;
	LogLevel maxFileLogLevel;
	LogLevel maxScreenLogLevel;
	QFile * logFile;
};

inline void Log::Debug(QString const& data)
{
	Log::LogToFile(data, Log::Log_Debug);
}

inline void Log::Warning(QString const& data)
{
	Log::LogToFile(data, Log::Log_Warn);
}

inline void Log::Error(QString const& data)
{
	LogToFile(data, Log::Log_Error);
}

inline void Log::Info(QString const& data)
{
	LogToFile(data, Log::Log_Info);
}
#endif
