#include <QDateTime>
#include <QMapIterator>
#include "bunny.h"
#include "bunnymanager.h"
#include "cron.h"
#include "messagepacket.h"
#include "plugin_clock.h"
#include "ttsmanager.h"

Q_EXPORT_PLUGIN2(plugin_clock, PluginClock)

PluginClock::PluginClock():PluginInterface("clock", "Clock")
{
	Cron::Register(this, 60, 0, 0, NULL);
}

PluginClock::~PluginClock()
{
	Cron::UnregisterAll(this);
}

bool PluginClock::Init()
{
	QDir * dir = GetLocalHTTPFolder();
	if(dir)
	{
		clockFolder = *dir;
		delete dir;
		return true;
	}
	// Init failed
	return false;
}

void PluginClock::OnCron(Bunny *, QVariant)
{
	foreach(Bunny * b, bunnyList)
	{
		if(b->IsIdle())
		{
			QString hour = QDateTime::currentDateTime().toString("h");
			TTSManager::CreateNewSound("Il est "+hour+" heure", "julie", clockFolder.absoluteFilePath(hour+".mp3"));
			QByteArray message = "MU "+GetBroadcastHTTPPath(hour+".mp3")+"\nPL 3\nMW\n";
			b->SendPacket(MessagePacket(message));
		}
	}
}

void PluginClock::OnBunnyConnect(Bunny * b)
{
	bunnyList.append(b);
}

void PluginClock::OnBunnyDisconnect(Bunny * b)
{
	bunnyList.removeAll(b);
}
