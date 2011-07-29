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
	enum State { State_Disconnected, State_Booting, State_Authenticating, State_Authenticated, State_Ready};
	virtual ~Bunny();

	static void Init() { InitApiCalls(); }

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

	QVariant GetPluginSetting(QString const&, QString const&, QVariant const& defaultValue = QVariant()) const;
	void SetPluginSetting(QString const&, QString const&, QVariant const&);
	void RemovePluginSetting(QString const&, QString const&);

	QVariant GetGlobalSetting(QString const&, QVariant const& defaultValue = QVariant()) const;
	void SetGlobalSetting(QString const&, QVariant const&);
	void RemoveGlobalSetting(QString const&);

	bool HasPlugin(PluginInterface * p) const;
	QList<QString> GetListOfPlugins();

	void XmppBunnyMessage(QByteArray const&);
	void XmppVioletMessage(QByteArray const&);
	bool XmppVioletPacketMessage(Packet const& p);

	void Booting();
	void Authenticating();
	void Authenticated();
	void Ready();

	bool IsAuthenticated() const;
	bool IsConnected() const;

	bool IsIdle() const;
	bool IsSleeping() const;

	QByteArray GetInitPacket() const;

	bool OnClick(PluginInterface::ClickType);
	bool OnEarsMove(int, int);
	bool OnRFID(QByteArray const&);

	void PluginStateChanged(PluginInterface * p);
	void PluginLoaded(PluginInterface *);
	void PluginUnloaded(PluginInterface *);

	// API
	static void InitApiCalls();
	ApiManager::ApiAnswer * ProcessVioletApiCall(HTTPRequest const&);

private slots:
	void SaveConfig();

private:
	Bunny(QByteArray const&);
	void LoadConfig();
	void AddPlugin(PluginInterface * p);
	void RemovePlugin(PluginInterface * p);
	void OnConnect();
	void OnDisconnect();

	QString CheckPlugin(PluginInterface *, bool isAssociated = false);

	// API
	API_CALL(Api_AddPlugin);
	API_CALL(Api_RemovePlugin);
	API_CALL(Api_GetListOfAssociatedPlugins);
	API_CALL(Api_SetSingleClickPlugin);
	API_CALL(Api_SetDoubleClickPlugin);
	API_CALL(Api_GetClickPlugins);
	API_CALL(Api_GetListOfKnownRFIDTags);
	API_CALL(Api_SetRFIDTagName);
	API_CALL(Api_SetBunnyName);
	API_CALL(Api_SetService);
	API_CALL(Api_ResetPassword);
	API_CALL(Api_ResetOwner);
	API_CALL(Api_Disconnect);
	API_CALL(Api_enableVApi);
	API_CALL(Api_disableVApi);
	API_CALL(Api_getVApiStatus);
	API_CALL(Api_getVApiToken);
	API_CALL(Api_setVApiToken);

	enum State state;

	QByteArray id;
	QByteArray xmppResource;
	QString configFileName;
	QHash<QString, QVariant> GlobalSettings;
	QHash<QString, QHash<QString, QVariant> > PluginsSettings;
	QList<QString> listOfPlugins;
	QList<PluginInterface*> listOfPluginsPtr;
	QTimer * saveTimer;
	XmppHandler * xmppHandler;

	PluginInterface * singleClickPlugin;
	PluginInterface * doubleClickPlugin;


	// RFID Tags
	QHash<QByteArray, QString> knownRFIDTags;
};

inline QList<QString> Bunny::GetListOfPlugins()
{
	return listOfPlugins;
}

inline bool Bunny::IsIdle() const
{
	return IsConnected() && ((bool)(xmppResource == "idle"));
}

inline bool Bunny::IsSleeping() const
{
	return IsConnected() && ((bool)(xmppResource == "asleep"));
}

inline bool Bunny::IsConnected() const
{
	return state == State_Ready;
}

inline bool Bunny::IsAuthenticated() const
{
	return (state == State_Ready) || (state == State_Authenticated);
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
	RemoveGlobalSetting("BunnyPassword");
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
