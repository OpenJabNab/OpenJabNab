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
	static inline void Init() { if(!instance) instance = new Log(); }
	static inline void Close() { if(instance) { delete instance; } }
	~Log();
	enum LogLevel { Log_None = 0, Log_Info, Log_Error, Log_Warn, Log_Debug};
	static void LogToFile(QString const&, LogLevel);
	
private:
	Log();
	LogLevel GetLevel(QString const&);
	
	static Log * instance;
	QTextStream logStream;
	LogLevel maxFileLogLevel;
	LogLevel maxScreenLogLevel;
	QFile * logFile;
};

#define LogInfo(data) Log::LogToFile(data, Log::Log_Info)
#define LogError(data) Log::LogToFile(QString("%1 : %2").arg(Q_FUNC_INFO,data), Log::Log_Error)
#define LogWarning(data) Log::LogToFile(QString("%1 : %2").arg(Q_FUNC_INFO,data), Log::Log_Warn)
#define LogDebug(data) Log::LogToFile(QString("%1 : %2").arg(Q_FUNC_INFO,data), Log::Log_Debug)

#endif
