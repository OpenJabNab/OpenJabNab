#ifndef _BUNNYMANAGER_H_
#define _BUNNYMANAGER_H_

#include <QMap>
#include <QVector>
#include "global.h"

class Bunny;
class OJN_EXPORT BunnyManager
{
public:
	static Bunny * GetBunny(QByteArray const&);
	static QVector<Bunny *> GetConnectedBunnies();
	static void Close();
	
private:
	BunnyManager();
	static QMap<QByteArray, Bunny *> listOfBunnies;
};

#endif
