#ifndef _BUNNY_H_
#define _BUNNY_H_

#include <QByteArray>
#include <QMap>
#include <QString>
#include <QTimer>
#include <QVariant>
#include "global.h"
#include "packet.h"

class XmppHandler;
class OJN_EXPORT Bunny : QObject
{
	Q_OBJECT
public:
	enum State { Connected, Disconnected };
	Bunny(QByteArray const&);
	~Bunny();

	bool IsConnected() const { return state == Connected; };
	QByteArray GetID() const { return id.toHex(); };
	void SetXmppHandler (XmppHandler *);
	void RemoveXmppHandler (XmppHandler *);
	void SendPacket(Packet const&);
	QVariant GetGlobalSetting(QString const&, QVariant const& defaultValue = QVariant()) const;
	void SetGlobalSetting(QString const&, QVariant const&);
	QVariant GetPluginSetting(QString const&, QString const&, QVariant const& defaultValue = QVariant()) const;
	void SetPluginSetting(QString const&, QString const&, QVariant const&);

private slots:
	void SaveConfig();
	
private:
	void LoadConfig();

	enum State state;
	QByteArray id;
	QString configFileName;
	QMap<QString, QVariant> GlobalSettings;
	QMap<QString, QMap<QString, QVariant> > PluginsSettings;
	QTimer * saveTimer;
	XmppHandler * xmppHandler;
};

#endif
