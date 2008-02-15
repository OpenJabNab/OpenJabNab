#ifndef _PLUGININTERFACE_H_
#define _PLUGININTERFACE_H_

#include <QByteArray>
#include <QSettings>
#include <QtPlugin>
#include <QString>
#include <QCoreApplication>
#include <QDir>
#include "httprequest.h"
#include "packet.h"

class PluginInterface
{
public:
	enum ClickType { SingleClick = 0, DoubleClick};

	PluginInterface(QString pluginName) {
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

	virtual bool OnClick(ClickType) { return false; };
	virtual bool OnEarsMove(int, int) { return false; };

protected:
	QSettings * settings;
};

Q_DECLARE_INTERFACE(PluginInterface,"org.toms.openjabnab.PluginInterface/1.0")

#endif
