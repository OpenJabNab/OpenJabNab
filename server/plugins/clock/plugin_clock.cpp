#include <QDateTime>
#include <QMapIterator>
#include "bunny.h"
#include "bunnymanager.h"
#include "cron.h"
#include "messagepacket.h"
#include "plugin_clock.h"
#include "ttsmanager.h"

Q_EXPORT_PLUGIN2(plugin_clock, PluginClock)

PluginClock::PluginClock():PluginInterface("clock", "Clock",BunnyPlugin)
{
	Cron::Register(this, 60, 0, 0, NULL);
	// Check available folders
	QDir * httpFolder = GetLocalHTTPFolder();
	if(httpFolder)
	{
		availableVoices = httpFolder->entryList(QDir::Dirs|QDir::NoDotAndDotDot);
		delete httpFolder;
	}
	availableVoices.push_back("tts");
}

PluginClock::~PluginClock()
{
	Cron::UnregisterAll(this);
}

void PluginClock::OnCron(Bunny *, QVariant)
{
	QMapIterator<Bunny *, QString> i(bunnyList);
	while (i.hasNext()) {
		i.next();
		Bunny * b = i.key();
		QString voice = i.value();
		if(b->IsIdle())
		{
			QString hour = QDateTime::currentDateTime().toString("h");
			QByteArray file;
			if(voice == "tts")
				file = TTSManager::CreateNewSound("Il est " + hour + " heure", "julie");
			else
			{
				// Fetch available files
				QDir * dir = GetLocalHTTPFolder();
				if(dir)
				{
					dir->cd(voice);
					dir->cd(hour);
					QStringList list = dir->entryList(QDir::Files|QDir::NoDotAndDotDot);
					if(list.count())
					{
						file = GetBroadcastHTTPPath(QString("%1/%2/%3").arg(voice, hour, list.at(qrand()%list.count())));
					}
					delete dir;
				}
				else
					LogError("Invalid GetLocalHTTPFolder()");
			}

			if(!file.isNull())
			{
				QByteArray message = "MU "+file+"\nPL 3\nMW\n";
				b->SendPacket(MessagePacket(message));
			}
		}

	}
}

void PluginClock::OnBunnyConnect(Bunny * b)
{
	QString voice = b->GetPluginSetting(GetName(), "voice", QString("tts")).toString();
	if(!availableVoices.contains(voice))
	{
		LogError(QString("Bunny '%1' has invalid voice '%2'").arg(b->GetID(), voice));
		voice = "tts";
	}
	bunnyList.insert(b, voice);
}

void PluginClock::OnBunnyDisconnect(Bunny * b)
{
	bunnyList.remove(b);
}

/*******
 * API *
 *******/

void PluginClock::InitApiCalls()
{
	DECLARE_PLUGIN_BUNNY_API_CALL("setVoice(name)", PluginClock, Api_SetVoice);
	DECLARE_PLUGIN_BUNNY_API_CALL("getVoiceList()", PluginClock, Api_GetVoiceList);
}

PLUGIN_BUNNY_API_CALL(PluginClock::Api_SetVoice)
{
	Q_UNUSED(account);

	QString voice = hRequest.GetArg("name");
	if(availableVoices.contains(voice))
	{
		// Update cache, set new voice
		bunnyList[bunny] = voice;
		// Save new config
		bunny->SetPluginSetting(GetName(), "voice", voice);

		return new ApiManager::ApiOk(QString("Voices changed to '%1'").arg(voice));
	}
	return new ApiManager::ApiError(QString("Unknown '%1' voice").arg(voice));
}

PLUGIN_BUNNY_API_CALL(PluginClock::Api_GetVoiceList)
{
	Q_UNUSED(account);
	Q_UNUSED(bunny);
	Q_UNUSED(hRequest);

	return new ApiManager::ApiList(availableVoices);
}
