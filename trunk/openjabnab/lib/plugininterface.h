#ifndef _PLUGININTERFACE_H_
#define _PLUGININTERFACE_H_

#include <QByteArray>
#include <QCoreApplication>
#include <QDir>
#include <QSettings>
#include <QString>
#include <QtPlugin>
#include "bunny.h"
#include "log.h"

class HTTPRequest;
class Packet;
class PluginInterface : public QObject
{
public:
	enum ClickType { SingleClick = 0, DoubleClick};
	enum PluginType { RequiredPlugin, SystemPlugin, BunnyPlugin};

	PluginInterface(QString name, QString visualName = QString(), int type = BunnyPlugin) {
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
	};
	virtual ~PluginInterface() { delete settings;};

	virtual void HttpRequestBefore(HTTPRequest const&) {};
	// If the plugin returns true, the plugin should handle the request
	virtual bool HttpRequestHandle(HTTPRequest &) { return false; };
	virtual void HttpRequestAfter(HTTPRequest const&) {};
	
	virtual void XmppBunnyMessage(QByteArray const&) {};
	virtual void XmppVioletMessage(QByteArray const&) {};
	// If the plugin returns true, the packet will be dropped
	virtual bool XmppVioletPacketMessage(Packet const&) { return false; };

	virtual bool OnClick(Bunny *, ClickType) { return false; };
	virtual bool OnEarsMove(Bunny *, int, int) { return false; };
	virtual bool OnRFID(Bunny *, QByteArray const&) { return false; };
	
	virtual void OnCron(QVariant) {};
	
	inline QVariant GetSettings(QString const& key, QVariant const& defaultValue = QVariant()) const { return settings->value(key, defaultValue); };
	void SetSettings(QString const& key, QVariant const& value) { settings->setValue(key, value); settings->sync(); };

	inline QString const& GetName() const { return pluginName; }
	inline QString const& GetVisualName() const { return pluginVisualName; }

	// Plugin enable/disable functions
	inline bool GetEnable() { return (pluginEnable && (bool)(GetRegisteredBunnies().count()) || pluginType != BunnyPlugin); }
	bool GetEnable(Bunny * b) { return (pluginEnable && HasBunny(b->GetID()) && (b->GetPluginSetting(pluginName, "pluginStatus/Enable", QVariant(true)).toBool()) || pluginType != BunnyPlugin); }
	void SetEnable(bool newStatus) {
		if(GetType() == RequiredPlugin)
		{
			Log::Info("Plugin "+GetVisualName()+" is already enabled");
		}
		else
		{
			pluginEnable = newStatus;
			SetSettings("pluginStatus/Enable", QVariant(newStatus)); 
			Log::Info("Plugin "+GetVisualName()+" is now "+(GetEnable() ? "enabled" : "disabled"));
		}
	}

	// Plugin type
	int GetType() { return pluginType; };

	// Bunny registration
	void RegisterBunny(Bunny * b) {
		listOfConnectedBunnies.append(b);
		Log::Info("Bunny "+b->GetID()+" is registering plugin "+GetVisualName());
		if(pluginType == SystemPlugin || b->GetPluginSetting(pluginName, "pluginStatus/Enable", QVariant(true)).toBool())
			SetEnable(true);
	};
	void UnregisterBunny(Bunny * b) {
		listOfConnectedBunnies.removeAt(listOfConnectedBunnies.indexOf(b));
		Log::Info("Bunny "+b->GetID()+" is unregistering plugin "+GetVisualName());
		if(pluginType != RequiredPlugin || GetRegisteredBunnies().count() == 0)
			SetEnable(false);
	};
	QList < Bunny* > GetRegisteredBunnies() { return listOfConnectedBunnies; };
	bool HasBunny(QByteArray BunnyID) {
		foreach(Bunny * b, listOfConnectedBunnies)
			if(b->GetID() == BunnyID)
				return true;
		return false;
	};

protected:
	QSettings * settings;

private:
	int pluginType;
	QString pluginName;
	QString pluginVisualName;
	bool pluginEnable;
	QList < Bunny* > listOfConnectedBunnies;
};

Q_DECLARE_INTERFACE(PluginInterface,"org.toms.openjabnab.PluginInterface/1.0")

#endif
