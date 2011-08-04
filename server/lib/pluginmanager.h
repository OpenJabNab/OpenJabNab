#ifndef _PLUGINMANAGER_H_
#define _PLUGINMANAGER_H_

#include <QMap>
#include <QList>
#include "global.h"
#include "plugininterface.h"
#include "apihandler.h"
#include "apimanager.h"

class Account;
class PluginInterface;
class PluginAuthInterface;
class QPluginLoader;
class OJN_EXPORT PluginManager : public ApiHandler<PluginManager>
{
public:
	static PluginManager & Instance();
	static void Init();
	static void Close();

	int GetEnabledPluginCount();
	int GetPluginCount();

	// HttpRequests are sent to all 'active' plugins
	void HttpRequestBefore(HTTPRequest &);
	bool HttpRequestHandle(HTTPRequest &);
	void HttpRequestAfter(HTTPRequest &);

	void XmppBunnyMessage(Bunny *, QByteArray const&);

	bool OnClick(Bunny *, PluginInterface::ClickType);
	bool OnEarsMove(Bunny *, int, int);
	bool OnRFID(Bunny *, QByteArray const&);
	bool OnRFID(Ztamp *, Bunny *);

	void OnBunnyConnect(Bunny *);
	void OnBunnyDisconnect(Bunny *);

	void OnZtampConnect(Ztamp *);
	void OnZtampDisconnect(Ztamp *);

	QList<PluginInterface *> const& GetListOfPlugins() const;
	PluginInterface * GetPluginByName(QString const& name) const;

	// API
	static void InitApiCalls();

	// Required Plugins
	// Auth
	void RegisterAuthPlugin(PluginAuthInterface *);
	void UnregisterAuthPlugin(PluginAuthInterface *);
	PluginAuthInterface * GetAuthPlugin() const;

private:
	PluginManager();
	void LoadPlugins();
	void UnloadPlugins();
	bool LoadPlugin(QString const&);
	bool UnloadPlugin(QString const&);
	bool ReloadPlugin(QString const&);
	QDir pluginsDir;
	QList<PluginInterface *> listOfPlugins;
	QList<PluginInterface *> listOfSystemPlugins;
	QMap<PluginInterface *, QString> listOfPluginsFileName;
	QMap<PluginInterface *, QPluginLoader *> listOfPluginsLoader;
	QHash<QString, PluginInterface *> listOfPluginsByName;
	QHash<QString, PluginInterface *> listOfPluginsByFileName;

	PluginAuthInterface * authPlugin;

	// API
	API_CALL(Api_GetListOfPlugins);
	API_CALL(Api_GetListOfEnabledPlugins);

	API_CALL(Api_GetListOfBunnyPlugins);
	API_CALL(Api_GetListOfBunnyEnabledPlugins);

	API_CALL(Api_GetListOfZtampPlugins);
	API_CALL(Api_GetListOfZtampEnabledPlugins);

	API_CALL(Api_GetListOfRequiredPlugins);
	API_CALL(Api_GetListOfSystemPlugins);
	API_CALL(Api_GetListOfSystemEnabledPlugins);

	API_CALL(Api_ActivatePlugin);
	API_CALL(Api_DeactivatePlugin);
	API_CALL(Api_LoadPlugin);
	API_CALL(Api_UnloadPlugin);
	API_CALL(Api_ReloadPlugin);
};

inline void PluginManager::Init()
{
	Instance().LoadPlugins();
	InitApiCalls();
}

inline void PluginManager::Close()
{
	Instance().UnloadPlugins();
}

inline QList<PluginInterface *> const& PluginManager::GetListOfPlugins() const
{
	return listOfPlugins;
}

inline PluginInterface * PluginManager::GetPluginByName(QString const& name) const
{
	return listOfPluginsByName.value(name);
}

#include "pluginauthinterface.h"

inline PluginAuthInterface * PluginManager::GetAuthPlugin() const
{
	if(authPlugin)
		return authPlugin;
	else
	{
		LogWarning("No Auth Plugin available, authentication is not possible for the moment");
		return PluginAuthInterface::DummyPlugin();
	}
}

#endif
