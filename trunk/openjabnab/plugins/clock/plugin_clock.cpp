#include <QDateTime>
#include <QCryptographicHash>
#include <QXmlStreamReader>
#include <QHttp>
#include <QMapIterator>
#include <QRegExp>
#include <QUrl>
#include <memory>
#include "bunny.h"
#include "bunnymanager.h"
#include "httprequest.h"
#include "cron.h"
#include "messagepacket.h"
#include "plugin_clock.h"
#include "settings.h"
#include "ttsmanager.h"

Q_EXPORT_PLUGIN2(plugin_clock, PluginClock)

PluginClock::PluginClock():PluginInterface("clock", "Clock")
{
	std::auto_ptr<QDir> dir(GetLocalHTTPFolder());
	if(dir.get())
	{
		clockFolder = *dir;
	}
}

PluginClock::~PluginClock()
{
	Cron::UnregisterAll(this);
}

void PluginClock::OnCron(QVariant v)
{
	Bunny * b = QVariantHelper::ToBunnyPtr(v);
	if(b->IsIdle())
	{
		QString hour = QDateTime::currentDateTime().toString("h");
		TTSManager::CreateNewSound("Il est "+hour+" heure", "julie", clockFolder.absoluteFilePath(hour+".mp3"));
		QByteArray message = "MU "+GetBroadcastHTTPPath(hour+".mp3")+"\nPL 3\nMW\n";
		b->SendPacket(MessagePacket(message));
	}
}

void PluginClock::OnBunnyConnect(Bunny * b)
{
	if(!webcastList.contains(b))
	{
		int id = Cron::Register(this, 60, 0, 0, QVariant::fromValue( b ));
		webcastList.insert(b , id);
	}
}

void PluginClock::OnBunnyDisconnect(Bunny * b)
{
	int id = webcastList.value(b);
	Cron::Unregister(this, id);
	webcastList.remove(b);
}
