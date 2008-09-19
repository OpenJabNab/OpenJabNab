#ifndef _BUNNYMANAGER_H_
#define _BUNNYMANAGER_H_

#include <QMap>
#include <QVector>
#include "global.h"
#include "apimanager.h"

class Bunny;
class HTTPRequest;
class OJN_EXPORT BunnyManager
{
public:
	static Bunny * GetBunny(QByteArray const&);
	static Bunny * GetConnectedBunny(QByteArray const&);
	static QVector<Bunny *> GetConnectedBunnies();
	static ApiManager::ApiAnswer * ProcessApiCall(QByteArray const& request, HTTPRequest const& hRequest);
	static void Close();
	
private:
	BunnyManager();
	static QMap<QByteArray, Bunny *> listOfBunnies;
};

#endif
