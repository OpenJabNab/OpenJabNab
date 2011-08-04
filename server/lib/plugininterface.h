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
#include "ztampmanager.h"
#include "log.h"
#include "pluginapihandler.h"
#include "settings.h"

class Account; 
class AmbientPacket;
class Bunny;
class HTTPRequest;
class Packet;
class SleepPacket;

class PluginInterface : public QObject, public PluginApiHandler
{
	friend class PluginManager;
public:
	enum ClickType { SingleClick = 0, DoubleClick};
	enum PluginType { RequiredPlugin, SystemPlugin, BunnyPlugin, ZtampPlugin, BunnyZtampPlugin};

	PluginInterface(QString name, QString visualName = QString(), PluginType type = BunnyPlugin);
	virtual ~PluginInterface();
	
	// Called to init plugin, return false if something is wrong
	virtual bool Init() { return true; };

	virtual void HttpRequestBefore(HTTPRequest &) {}
	// If the plugin returns true, the plugin should handle the request
	virtual bool HttpRequestHandle(HTTPRequest &) { return false; }
	virtual void HttpRequestAfter(HTTPRequest &) { }
	
	// Raw XMPP Messages
	virtual void XmppBunnyMessage(Bunny *, QByteArray const&) {}

	// Bunny's Messages
	virtual void OnInitPacket(const Bunny *, AmbientPacket &, SleepPacket &) {}
	virtual bool OnClick(Bunny *, ClickType) { return false; }
	virtual bool OnEarsMove(Bunny *, int, int) { return false; }
	virtual bool OnRFID(Bunny *, QByteArray const&) { return false; }
	virtual bool OnRFID(Ztamp *, Bunny *) { return false; }

	// Cron system
	virtual void OnCron(Bunny*, QVariant) {}

	// Ztamp connect/disconnect
	virtual void OnZtampConnect(Ztamp *) {}
	virtual void OnZtampDisconnect(Ztamp *) {}
	
	// Bunny connect/disconnect
	virtual void OnBunnyConnect(Bunny *) {}
	virtual void OnBunnyDisconnect(Bunny *) {}
	
	// Settings
	QVariant GetSettings(QString const& key, QVariant const& defaultValue = QVariant()) const;
	void SetSettings(QString const& key, QVariant const& value);

	// Plugin's name
	QString const& GetName() const;
	QString const& GetVisualName() const;

	// Plugin enable/disable functions
	bool GetEnable() const;

	// Plugin type
	int GetType() const;

protected:
	void SetEnable(bool);
	QDir * GetLocalHTTPFolder() const;
	QByteArray GetBroadcastHTTPPath(QString f) const;

	QSettings * settings;

private:
	QString pluginName;
	PluginType pluginType;
	QString pluginVisualName;
	bool pluginEnable;
	QString httpFolder;
};

#include "plugininterface_inline.h"

Q_DECLARE_INTERFACE(PluginInterface,"org.toms.openjabnab.PluginInterface/1.0")

#endif
