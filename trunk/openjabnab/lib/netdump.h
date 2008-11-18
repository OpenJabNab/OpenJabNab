#ifndef _PLUGINDUMP_H_
#define _PLUGINDUMP_H_

#include <QTextStream>
#include "httprequest.h"
	
class OJN_EXPORT NetworkDump
{

public:
	static NetworkDump& Instance();
	static void Init();
	static void Close();
	static void Log(QString const& what, QString const& txt);
	
private:
	NetworkDump() {};
	virtual ~NetworkDump() {};

	QTextStream dumpStream;
};

#endif
