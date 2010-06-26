#ifndef _BUNNY_H_
#define _BUNNY_H_

#include <QByteArray>
#include <QHash>
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

	bool IsConnected() const;
	bool IsAuthenticated() const;
	bool IsIdle() const;
	
	QByteArray GetID() const;
	void SetXmppHandler (XmppHandler *);
	void RemoveXmppHandler (XmppHandler *);
	void SendPacket(Packet const&);
	void SendData(QByteArray const&);

	QString GetBunnyName() const;
	void SetBunnyName(QString const& bunnyName);
	QByteArray GetBunnyPassword() const;
	bool SetBunnyPassword(QByteArray const& bunnyName);
	bool ClearBunnyPassword();

	QByteArray GetXmppResource() const;
	void SetXmppResource(QByteArray const&);

	QVariant GetGlobalSetting(QString const&, QVariant const& defaultValue = QVariant()) const;
	void SetGlobalSetting(QString const&, QVariant const&);

	QVariant GetPluginSetting(QString const&, QString const&, QVariant const& defaultValue = QVariant()) const;
	void SetPluginSetting(QString const&, QString const&, QVariant const&);

	bool HasPlugin(PluginInterface * p) const;
	QList<QString> GetListOfPlugins();

	void XmppBunnyMessage(QByteArray const&);
	void XmppVioletMessage(QByteArray const&);
	bool XmppVioletPacketMessage(Packet const& p);
	
	bool OnClick(PluginInterface::ClickType);
	bool OnEarsMove(int, int);
	bool OnRFID(QByteArray const&);

	void PluginStateChanged(PluginInterface * p);
	void PluginLoaded(PluginInterface *);
	void PluginUnloaded(PluginInterface *);

	// API
	static void InitApiCalls();

private slots:
	void SaveConfig();
	
private:
	Bunny(QByteArray const&);
	void LoadConfig();
	void AddPlugin(PluginInterface * p);
	void RemovePlugin(PluginInterface * p);
	void OnConnect();
	void OnDisconnect();

	// API
	API_CALL(Api_RegisterPlugin);
	API_CALL(Api_UnregisterPlugin);
	API_CALL(Api_GetListOfActivePlugins);
	API_CALL(Api_SetSingleClickPlugin);
	API_CALL(Api_SetDoubleClickPlugin);
	API_CALL(Api_GetClickPlugins);

	enum State state;
	bool isAuthenticated;
	
	QByteArray id;
	QByteArray xmppResource;
	QString configFileName;
	QHash<QString, QVariant> GlobalSettings;
	QHash<QString, QHash<QString, QVariant> > PluginsSettings;
	QList<QString> listOfPlugins;
	QList<PluginInterface*> listOfPluginsPtr;
	QTimer * saveTimer;
	XmppHandler * xmppHandler;
};

inline QList<QString> Bunny::GetListOfPlugins()
{
	return listOfPlugins;
}

inline bool Bunny::IsIdle() const
{
	return (bool)(xmppResource == "idle");
}

inline bool Bunny::IsConnected() const
{
	return state == Connected;
}

inline bool Bunny::IsAuthenticated() const
{
	return isAuthenticated;
}

inline QByteArray Bunny::GetID() const
{
	return id.toHex(); 
}

inline QByteArray Bunny::GetXmppResource() const
{
	return xmppResource; 
}

inline void Bunny::SetXmppResource(QByteArray const& r)
{
	xmppResource = r;
}

inline QString Bunny::GetBunnyName() const
{
	return GetGlobalSetting("BunnyName", "Bunny").toString();
}

inline void Bunny::SetBunnyName(QString const& bunnyName)
{
	SetGlobalSetting("BunnyName", bunnyName);
}

inline QByteArray Bunny::GetBunnyPassword() const
{
	return GetGlobalSetting("BunnyPassword", QByteArray()).toByteArray();
}

inline bool Bunny::ClearBunnyPassword()
{
	SetGlobalSetting("BunnyPassword", "");
	return true;
}
inline bool Bunny::SetBunnyPassword(QByteArray const& bunnyPassword)
{
	if(Bunny::GetBunnyPassword() == QByteArray())
	{
		SetGlobalSetting("BunnyPassword", bunnyPassword);
		return true;
	}
	return false;
}
inline bool Bunny::HasPlugin(PluginInterface * p) const
{
	return listOfPluginsPtr.contains(p);
}

Q_DECLARE_METATYPE(Bunny*)
namespace QVariantHelper
{
    inline Bunny* ToBunnyPtr(QVariant v) { return v.value<Bunny*>(); }
};
#endif
