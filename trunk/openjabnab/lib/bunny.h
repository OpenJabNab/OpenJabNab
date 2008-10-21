#ifndef _BUNNY_H_
#define _BUNNY_H_

#include <QByteArray>
#include <QMap>
#include <QString>
#include <QTimer>
#include <QVariant>
#include "apihandler.h"
#include "apimanager.h"
#include "global.h"
#include "packet.h"
#include "plugininterface.h"

class XmppHandler;
class OJN_EXPORT Bunny : QObject, public ApiHandler<Bunny>
{
	friend class BunnyManager;
	Q_OBJECT
public:
	enum State { Connected, Disconnected };
	virtual ~Bunny();

	bool IsConnected() const { return state == Connected; };
	QByteArray GetID() const { return id.toHex(); };
	void SetXmppHandler (XmppHandler *);
	void RemoveXmppHandler (XmppHandler *);
	void SendPacket(Packet const&);

	QString GetBunnyName() { return GetGlobalSetting("BunnyName", "Bunny").toString(); };
	void SetBunnyName(QVariant const& bunnyName) {SetGlobalSetting("BunnyName", bunnyName); };

	QVariant GetGlobalSetting(QString const&, QVariant const& defaultValue = QVariant()) const;
	void SetGlobalSetting(QString const&, QVariant const&);

	QVariant GetPluginSetting(QString const&, QString const&, QVariant const& defaultValue = QVariant()) const;
	void SetPluginSetting(QString const&, QString const&, QVariant const&);

	inline bool HasPlugin(PluginInterface * p) { return listOfPluginsPtr.contains(p); };

	void XmppBunnyMessage(QByteArray const&);
	void XmppVioletMessage(QByteArray const&);
	bool XmppVioletPacketMessage(Packet const& p);
	
	bool OnClick(PluginInterface::ClickType);
	bool OnEarsMove(int, int);
	bool OnRFID(QByteArray const&);

	void PluginStateChanged(PluginInterface * p);
	void PluginLoaded(PluginInterface *);
	void PluginUnloaded(PluginInterface *);

	ApiManager::ApiAnswer * ProcessApiCall_RegisterPlugin(Account const&, QString const&, HTTPRequest const& hRequest);
	ApiManager::ApiAnswer * ProcessApiCall_UnregisterPlugin(Account const&, QString const&, HTTPRequest const& hRequest);
	ApiManager::ApiAnswer * ProcessApiCall_GetListOfActivePlugins(Account const&, QString const&, HTTPRequest const& hRequest);

private slots:
	void SaveConfig();
	
private:
	Bunny(QByteArray const&);
	void LoadConfig();
	void AddPlugin(PluginInterface * p);
	void RemovePlugin(PluginInterface * p);
	void OnConnect();
	void OnDisconnect();

	enum State state;
	QByteArray id;
	QString configFileName;
	QMap<QString, QVariant> GlobalSettings;
	QMap<QString, QMap<QString, QVariant> > PluginsSettings;
	QList<QString> listOfPlugins;
	QList<PluginInterface*> listOfPluginsPtr;
	QTimer * saveTimer;
	XmppHandler * xmppHandler;
};

Q_DECLARE_METATYPE(Bunny*)
namespace QVariantHelper
{
    inline Bunny* ToBunnyPtr(QVariant v) { return v.value<Bunny*>(); }
};
#endif
