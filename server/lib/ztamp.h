#ifndef _ZTAMP_H_
#define _ZTAMP_H_

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

//class XmppHandler;
class OJN_EXPORT Ztamp : QObject, public ApiHandler<Ztamp>
{
	friend class ZtampManager;
	Q_OBJECT
public:
	virtual ~Ztamp();

	static void Init() { InitApiCalls(); }

	QByteArray GetID() const;
	QString GetZtampName() const;
	void SetZtampName(QString const& ztampName);

	QVariant GetPluginSetting(QString const&, QString const&, QVariant const& defaultValue = QVariant()) const;
	void SetPluginSetting(QString const&, QString const&, QVariant const&);
	void RemovePluginSetting(QString const&, QString const&);

	bool HasPlugin(PluginInterface * p) const;
	QList<QString> GetListOfPlugins();

	bool OnRFID(Bunny *);

	void PluginStateChanged(PluginInterface * p);
	void PluginLoaded(PluginInterface *);
	void PluginUnloaded(PluginInterface *);

	QVariant GetGlobalSetting(QString const&, QVariant const& defaultValue = QVariant()) const;
	void SetGlobalSetting(QString const&, QVariant const&);
	void RemoveGlobalSetting(QString const&);

	// API
	static void InitApiCalls();

private slots:
	void SaveConfig();

private:
	Ztamp(QByteArray const&);
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
	API_CALL(Api_SetZtampName);
	API_CALL(Api_RemoveOwner);
	API_CALL(Api_ResetOwner);

	QByteArray id;
	QString configFileName;
	QHash<QString, QVariant> GlobalSettings;
	QHash<QString, QHash<QString, QVariant> > PluginsSettings;
	QList<QString> listOfPlugins;
	QList<PluginInterface*> listOfPluginsPtr;
	QTimer * saveTimer;

	// RFID Tags
};

inline QList<QString> Ztamp::GetListOfPlugins()
{
	return listOfPlugins;
}

inline QByteArray Ztamp::GetID() const
{
	return id.toHex();
}

inline QString Ztamp::GetZtampName() const
{
	return GetGlobalSetting("ZtampName", "Ztamp").toString();
}

inline void Ztamp::SetZtampName(QString const& ztampName)
{
	SetGlobalSetting("ZtampName", ztampName);
}

inline bool Ztamp::HasPlugin(PluginInterface * p) const
{
	return listOfPluginsPtr.contains(p);
}

Q_DECLARE_METATYPE(Ztamp*)
namespace QVariantHelper
{
    inline Ztamp* ToZtampPtr(QVariant v) { return v.value<Ztamp*>(); }
};
#endif
