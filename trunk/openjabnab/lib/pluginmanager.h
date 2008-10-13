#ifndef _PLUGINMANAGER_H_
#define _PLUGINMANAGER_H_

#include <QMap>
#include <QList>
#include "global.h"
#include "plugininterface.h"
#include "apimanager.h"

class PluginInterface;
class QPluginLoader;
class OJN_EXPORT PluginManager
{
public:
	static PluginManager & Instance();
	static inline void Init() { Instance().LoadPlugins(); };
	static inline void Close() { Instance().UnloadPlugins(); };

	// HttpRequests are sent to all 'active' plugins
	void HttpRequestBefore(HTTPRequest const&);
	bool HttpRequestHandle(HTTPRequest &);
	void HttpRequestAfter(HTTPRequest const&);
	
	void XmppBunnyMessage(Bunny *, QByteArray const&);
	void XmppVioletMessage(Bunny *, QByteArray const&);
	bool XmppVioletPacketMessage(Bunny *, Packet const& p);
	
	bool OnClick(Bunny *, PluginInterface::ClickType);
	bool OnEarsMove(Bunny *, int, int);
	bool OnRFID(Bunny *, QByteArray const&);
	
	void OnBunnyConnect(Bunny *);
	void OnBunnyDisconnect(Bunny *);

	QList<PluginInterface *> const& GetListOfPlugins() { return listOfPlugins; }
	PluginInterface * GetPluginByName(QString name) { return listOfPluginsByName.value(name); }

	ApiManager::ApiAnswer * ProcessApiCall(QByteArray const& request, HTTPRequest const& hRequest);

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
	QMap<QString, PluginInterface *> listOfPluginsByName;
	QMap<QString, PluginInterface *> listOfPluginsByFileName;
};

#endif
