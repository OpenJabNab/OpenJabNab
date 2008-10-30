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

class Account; 
class Bunny;
class HTTPRequest;
class Packet;
class PluginInterface : public QObject
{
	friend class PluginManager;
public:
	enum ClickType { SingleClick = 0, DoubleClick};
	enum PluginType { RequiredPlugin, SystemPlugin, BunnyPlugin};

	PluginInterface(QString name, QString visualName = QString(), PluginType type = BunnyPlugin);
	virtual ~PluginInterface();

	virtual void HttpRequestBefore(HTTPRequest const&) {}
	// If the plugin returns true, the plugin should handle the request
	virtual bool HttpRequestHandle(HTTPRequest &) { return false; }
	virtual void HttpRequestAfter(HTTPRequest const&) {}
	
	// Raw XMPP Messages
	virtual void XmppBunnyMessage(Bunny *, QByteArray const&) {}
	virtual void XmppVioletMessage(Bunny *, QByteArray const&) {}

	// Violet's Packets
	// If the plugin returns true, the packet will be dropped
	virtual bool XmppVioletPacketMessage(Bunny *, Packet const&) { return false; }

	// Bunny's Messages
	virtual bool OnClick(Bunny *, ClickType) { return false; }
	virtual bool OnEarsMove(Bunny *, int, int) { return false; }
	virtual bool OnRFID(Bunny *, QByteArray const&) { return false; }

	// Cron system
	virtual void OnCron(QVariant) {}

	// Bunny connect/disconnect
	virtual void OnBunnyConnect(Bunny *) {}
	virtual void OnBunnyDisconnect(Bunny *) {}
	
	// Settings
	QVariant GetSettings(QString const& key, QVariant const& defaultValue = QVariant()) const;
	void SetSettings(QString const& key, QVariant const& value);

	// Plugin's name
	QString const& GetName() const;
	QString const& GetVisualName() const;

	// Api Call
	virtual ApiManager::ApiAnswer * ProcessApiCall(Account const&, QString const&, HTTPRequest const&);
	virtual ApiManager::ApiAnswer * ProcessBunnyApiCall(Bunny *, Account const&, QString const&, HTTPRequest const&);

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
