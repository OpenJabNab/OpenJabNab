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
	static inline void Init() { InitApiCalls(); };
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

private:
	BunnyManager();
	static QHash<QByteArray, Bunny *> listOfBunnies;
};

#endif
