#ifndef _PLUGINMANAGER_H_
#define _PLUGINMANAGER_H_

#include <QMap>
#include <QList>
#include "global.h"
#include "plugininterface.h"
#include "apimanager.h"

class PluginInterface;
class OJN_EXPORT PluginManager
{
public:
	static PluginManager & Instance();
	void LoadPlugins();
	virtual ~PluginManager();

	void HttpRequestBefore(HTTPRequest const&);
	bool HttpRequestHandle(HTTPRequest &);
	void HttpRequestAfter(HTTPRequest const&);
	
	void XmppBunnyMessage(QByteArray const&);
	void XmppVioletMessage(QByteArray const&);
	bool XmppVioletPacketMessage(Packet const& p);
	
	bool OnClick(Bunny *, PluginInterface::ClickType);
	bool OnEarsMove(Bunny *, int, int);
	bool OnRFID(Bunny *, QByteArray const&);
	
	QList<PluginInterface *> const& GetListOfPlugins() { return listOfPluginsPtr; }
	PluginInterface * GetPluginByName(QString name) { return listOfPluginsByName.value(name); }
	ApiManager::ApiAnswer * ProcessApiCall(QByteArray const& request, HTTPRequest const& hRequest);

private:
	PluginManager();
	bool LoadPlugin(QString const&);
	bool UnloadPlugin(QString const&);
	bool ReloadPlugin(QString const&);
	QDir pluginsDir;
	QList<PluginInterface *> listOfPluginsPtr;
	QMap<PluginInterface *, QString> listOfPluginsFileName;
	QMap<QString, PluginInterface *> listOfPluginsByName;
	QMap<QString, PluginInterface *> listOfPluginsByFileName;
};

#endif
