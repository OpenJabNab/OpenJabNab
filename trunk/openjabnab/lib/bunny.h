#ifndef _BUNNY_H_
#define _BUNNY_H_

#include "global.h"
#include "xmpphandler.h"
#include "packet.h"

#include <QByteArray>
#include <QString>
#include <QMap>
#include <QVariant>
#include <QTimer>

class OJN_EXPORT Bunny : QObject
{
	Q_OBJECT
public:
	Bunny(QByteArray const&);
	bool IsConnected() const { return state == Connected; };
	void SetXmppHandler (XmppHandler *);
	void SendPacket(Packet const&);
	QVariant GetGlobalSetting(QString const&, QVariant const& defaultValue = QVariant());
	void SetGlobalSetting(QString const&, QVariant &);
	QVariant GetPluginSetting(QString const&, QString const&, QVariant const& defaultValue = QVariant());
	void SetPluginSetting(QString const&, QString const&, QVariant &);

	~Bunny();
	enum State { Connected, Disconnected };

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
