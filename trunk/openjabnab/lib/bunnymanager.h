#ifndef _BUNNYMANAGER_H_
#define _BUNNYMANAGER_H_

#include "global.h"
#include "bunny.h"
#include <QMap>

class OJN_EXPORT BunnyManager
{
public:
	static Bunny * GetBunny(QByteArray const&);
	static void Close();
	
private:
	BunnyManager();
	static QMap<QByteArray, Bunny *> listOfBunnies;
};

#endif
