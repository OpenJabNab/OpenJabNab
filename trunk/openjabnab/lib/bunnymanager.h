#ifndef _BUNNYMANAGER_H_
#define _BUNNYMANAGER_H_

#include <QMap>
#include <QVector>
#include "global.h"
#include "apimanager.h"

class Account;
class Bunny;
class HTTPRequest;
class PluginInterface;
class OJN_EXPORT BunnyManager
{
	friend class XmppHandler;
	friend class ApiManager;
	friend class PluginManager;
public:
	static Bunny * GetBunny(PluginInterface *, QByteArray const&);
	static ApiManager::ApiAnswer * ProcessApiCall(Account const&, QByteArray const& request, HTTPRequest const& hRequest);
	static void PluginStateChanged(PluginInterface *);
	static void Close();

protected:
	static Bunny * GetBunny(QByteArray const&);
	static Bunny * GetConnectedBunny(QByteArray const&);
	static QVector<Bunny *> GetConnectedBunnies();
	static void PluginLoaded(PluginInterface *);
	static void PluginUnloaded(PluginInterface *);

private:
	BunnyManager();
	static QMap<QByteArray, Bunny *> listOfBunnies;
};

#endif
