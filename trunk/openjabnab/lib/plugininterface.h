#ifndef _PLUGININTERFACE_H_
#define _PLUGININTERFACE_H_

#include <QByteArray>
#include <QCoreApplication>
#include <QDir>
#include <QSettings>
#include <QString>
#include <QtPlugin>

class Bunny;
class HTTPRequest;
class Packet;
class PluginInterface
{
public:
	enum ClickType { SingleClick = 0, DoubleClick};

	PluginInterface(QString name) {
		pluginName = name;
		// Create settings object
		QDir dir = QDir(QCoreApplication::applicationDirPath());
		dir.cd("plugins");
		settings = new QSettings(dir.absoluteFilePath("plugin_"+pluginName+".ini"), QSettings::IniFormat);
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
	
	QVariant GetSettings(QString const& key, QVariant const& defaultValue = QVariant()) { return settings->value(key, defaultValue); };
	void SetSettings(QString const& key, QVariant const& value) { settings->setValue(key, value); settings->sync(); };
	
	QString const& GetName() { return pluginName; }

private:
	QSettings * settings;
	QString pluginName;
};

Q_DECLARE_INTERFACE(PluginInterface,"org.toms.openjabnab.PluginInterface/1.0")

#endif
