#ifndef _PLUGININTERFACE_H_
#define _PLUGININTERFACE_H_

#include <QByteArray>
#include <QCoreApplication>
#include <QDir>
#include <QSettings>
#include <QString>
#include <QtPlugin>
#include "bunny.h"

class HTTPRequest;
class Packet;
class PluginInterface : public QObject
{
public:
	enum ClickType { SingleClick = 0, DoubleClick};

	PluginInterface(QString name, QString visualName = QString()) {
		pluginName = name;
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
	inline bool GetEnable() const { return pluginEnable; }
	bool GetEnable(Bunny * b) const { return pluginEnable && (b->GetPluginSetting(pluginName, "pluginStatus/Enable", QVariant(true)).toBool()); }
	void SetEnable(bool newStatus) { pluginEnable = newStatus; SetSettings("pluginStatus/Enable", QVariant(newStatus)); }

private:
	QSettings * settings;
	QString pluginName;
	QString pluginVisualName;
	bool pluginEnable;
};

Q_DECLARE_INTERFACE(PluginInterface,"org.toms.openjabnab.PluginInterface/1.0")

#endif
