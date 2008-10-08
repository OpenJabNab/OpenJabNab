#ifndef _PLUGININTERFACE_H_
#define _PLUGININTERFACE_H_

#include <QByteArray>
#include <QCoreApplication>
#include <QDir>
#include <QSettings>
#include <QString>
#include <QtPlugin>
#include "apimanager.h"
#include "bunnymanager.h"
#include "log.h"
#include "settings.h"

class Bunny;
class HTTPRequest;
class Packet;
class PluginInterface : public QObject
{
	friend class PluginManager;
public:
	enum ClickType { SingleClick = 0, DoubleClick};
	enum PluginType { RequiredPlugin, SystemPlugin, BunnyPlugin};

	PluginInterface(QString name, QString visualName = QString(), PluginType type = BunnyPlugin) {
		pluginName = name;
		pluginType = type;
		// The visual name is more user-friendly (for visual-side only)
		if(visualName != QString())
			pluginVisualName = visualName;
		else
			pluginVisualName = name;
		// Create settings object
		QDir dir = QDir(QCoreApplication::applicationDirPath());
		dir.cd("plugins");
		settings = new QSettings(dir.absoluteFilePath("plugin_"+pluginName+".ini"), QSettings::IniFormat);
		pluginEnable = GetSettings("pluginStatus/Enable", QVariant(true)).toBool();
		// Compute Plugin's Http path
		httpFolder = QString("%1/%2/%3").arg(GlobalSettings::GetString("Config/HttpRoot"), GlobalSettings::GetString("Config/HttpPluginsFolder"), pluginName);
	};
	virtual ~PluginInterface() { delete settings;};

	virtual void HttpRequestBefore(HTTPRequest const&) {};
	// If the plugin returns true, the plugin should handle the request
	virtual bool HttpRequestHandle(HTTPRequest &) { return false; };
	virtual void HttpRequestAfter(HTTPRequest const&) {};
	
	// Raw XMPP Messages
	virtual void XmppBunnyMessage(Bunny *, QByteArray const&) {};
	virtual void XmppVioletMessage(Bunny *, QByteArray const&) {};

	// Violet's Packets
	// If the plugin returns true, the packet will be dropped
	virtual bool XmppVioletPacketMessage(Bunny *, Packet const&) { return false; };

	// Bunny's Messages
	virtual bool OnClick(Bunny *, ClickType) { return false; };
	virtual bool OnEarsMove(Bunny *, int, int) { return false; };
	virtual bool OnRFID(Bunny *, QByteArray const&) { return false; };

	// Cron system
	virtual void OnCron(QVariant) {};

	// Bunny connect/disconnect
	virtual void OnBunnyConnect(Bunny *) {};
	virtual void OnBunnyDisconnect(Bunny *) {};
	
	// Settings
	inline QVariant GetSettings(QString const& key, QVariant const& defaultValue = QVariant()) const { return settings->value(key, defaultValue); };
	void SetSettings(QString const& key, QVariant const& value) { settings->setValue(key, value); settings->sync(); };

	// Plugin's name
	inline QString const& GetName() const { return pluginName; }
	inline QString const& GetVisualName() const { return pluginVisualName; }

	// Api Call
	virtual ApiManager::ApiAnswer * ProcessApiCall(QByteArray const&, HTTPRequest const&) { return new ApiManager::ApiError(QString("This plugin doesn't support API")); };

	// Plugin enable/disable functions
	inline bool GetEnable() { return pluginEnable; }

	// Plugin type
	int GetType() { return pluginType; };

protected:
	// Plugin enable/disable functions
	void SetEnable(bool newStatus)
	{
		if(newStatus != pluginEnable)
		{
			pluginEnable = newStatus;
			SetSettings("pluginStatus/Enable", QVariant(newStatus)); 
			Log::Info("Plugin " + GetVisualName() + " is now " + (GetEnable() ? "enabled" : "disabled"));
			if(pluginType == BunnyPlugin)
				BunnyManager::PluginStateChanged(this);
		}
	}
	// HTTP Data folder
	QDir * GetLocalHTTPFolder() const
	{
		QDir pluginsFolder(GlobalSettings::GetString("Config/RealHttpRoot"));
		QString httpPluginsFolder = GlobalSettings::GetString("Config/HttpPluginsFolder");
		if (!pluginsFolder.cd(httpPluginsFolder))
		{
			if (!pluginsFolder.mkdir(httpPluginsFolder))
			{
				Log::Error(QString("Unable to create %1 directory !\n").arg(httpPluginsFolder));
				return NULL;
			}
			pluginsFolder.cd(httpPluginsFolder);
		}
		if (!pluginsFolder.cd(pluginName))
		{
			if (!pluginsFolder.mkdir(pluginName))
			{
				Log::Error(QString("Unable to create %1/%2 directory !\n").arg(httpPluginsFolder, pluginName));
				return NULL;
			}
			pluginsFolder.cd("tts");
		}
		return new QDir(pluginsFolder);
	}

	QByteArray GetBroadcastHTTPPath(QString f) const
	{
		return QString("broadcast/%1/%2").arg(httpFolder, f).toAscii();
	}
	QSettings * settings;

private:
	PluginType pluginType;
	QString pluginName;
	QString pluginVisualName;
	bool pluginEnable;
	QString httpFolder;
};

Q_DECLARE_INTERFACE(PluginInterface,"org.toms.openjabnab.PluginInterface/1.0")

#endif
