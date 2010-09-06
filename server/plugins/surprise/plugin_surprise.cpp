#include <QMapIterator>
#include "plugin_surprise.h"
#include "bunny.h"
#include "cron.h"
#include "messagepacket.h"

Q_EXPORT_PLUGIN2(plugin_surprise, PluginSurprise)

#define RANDOMIZE(x) x

PluginSurprise::PluginSurprise():PluginInterface("surprise", "Send random mp3 at random intervals") {}

PluginSurprise::~PluginSurprise() {}

void PluginSurprise::createCron(Bunny * b)
{
	// Check Frequency
	unsigned int frequency = b->GetPluginSetting(GetName(), "frequency", (uint)0).toUInt();
	if(!frequency)
	{
		LogError(QString("Bunny '%1' has invalid frequency '%2'").arg(b->GetID(), QString::number(frequency)));
		return;
	}
	
	// Register cron
	Cron::RegisterOneShot(this, RANDOMIZE(frequency), b, QVariant(), NULL);
}

void PluginSurprise::OnBunnyConnect(Bunny * b)
{
	createCron(b);
}

void PluginSurprise::OnBunnyDisconnect(Bunny * b)
{
	Cron::UnregisterAllForBunny(this, b);
}

void PluginSurprise::OnCron(Bunny * b, QVariant)
{
	if(b->IsIdle())
	{
		QByteArray file;
		// Fetch available files
		QDir * dir = GetLocalHTTPFolder();
		if(dir)
		{
			QString surprise = b->GetPluginSetting(GetName(), "list", QString()).toString();
			
			if(!surprise.isNull() && dir->cd(surprise))
			{
				QStringList list = dir->entryList(QDir::Files|QDir::NoDotAndDotDot);
				if(list.count())
				{
					file = GetBroadcastHTTPPath(QString("%1/%3").arg(surprise, list.at(qrand()%list.count())));
					QByteArray message = "MU "+file+"\nPL 3\nMW\n";
					b->SendPacket(MessagePacket(message));
				}
			}
			else
				LogError("Invalid surprise config");

			delete dir;
		}
		else
			LogError("Invalid GetLocalHTTPFolder()");
	}
	// Restart Timer
	createCron(b);
}