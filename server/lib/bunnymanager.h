#ifndef _BUNNYMANAGER_H_
#define _BUNNYMANAGER_H_

#include <QHash>
#include <QVector>
#include "global.h"
#include "apihandler.h"
#include "apimanager.h"

class Account;
class Bunny;
class HTTPRequest;
class PluginInterface;
class OJN_EXPORT BunnyManager : public ApiHandler<BunnyManager>
{
	friend class PluginAuth;
	friend class ApiManager;
	friend class PluginManager;
public:
	static BunnyManager & Instance();

	static Bunny * GetBunny(PluginInterface *, QByteArray const&);
	static void PluginStateChanged(PluginInterface *);
	static void Init();
	static void LoadBunnies();
	static void Close();

	// API
	static void InitApiCalls();

protected:
	static Bunny * GetBunny(QByteArray const&);
	static Bunny * GetConnectedBunny(QByteArray const&);
	static QVector<Bunny *> GetConnectedBunnies();
	static void PluginLoaded(PluginInterface *);
	static void PluginUnloaded(PluginInterface *);

	// API
	API_CALL(Api_GetListOfConnectedBunnies);
	API_CALL(Api_GetListOfBunnies);

private:
	BunnyManager();
	void LoadAllBunnies();
	QDir bunniesDir;
	static QHash<QByteArray, Bunny *> listOfBunnies;
	static QHash<QByteArray, Bunny *> allBunnies;
};

inline void BunnyManager::Init()
{
	InitApiCalls();
}

inline void BunnyManager::LoadBunnies()
{
	Instance().LoadAllBunnies();
}

#endif
