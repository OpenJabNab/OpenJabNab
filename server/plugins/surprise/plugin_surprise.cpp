#include <QMapIterator>
#include "plugin_surprise.h"
#include "bunny.h"
#include "cron.h"
#include "messagepacket.h"

Q_EXPORT_PLUGIN2(plugin_surprise, PluginSurprise)

// +/- 20% - 30min => rand(24,36)
#define RANDOMIZEDRATIO 20

PluginSurprise::PluginSurprise():PluginInterface("surprise", "Send random mp3 at random intervals",BunnyPlugin) {}

PluginSurprise::~PluginSurprise() {}

void PluginSurprise::createCron(Bunny * b)
{
	// Check Frequency
	unsigned int frequency = b->GetPluginSetting(GetName(), "frequency", (uint)0).toUInt();
	if(!frequency)
	{
		// Stable. Warning Removed.
		LogDebug(QString("Bunny '%1' has invalid frequency '%2'").arg(b->GetID(), QString::number(frequency)));
		return;
	}

	// Register cron
	Cron::RegisterOneShot(this, GetRandomizedFrequency(frequency), b, QVariant(), NULL);
}

bool PluginSurprise::OnClick(Bunny * b, PluginInterface::ClickType type)
{
        if (type == PluginInterface::SingleClick)
        {
		SendSurprise(b);
		return true;
        }
        return false;
}

int PluginSurprise::GetRandomizedFrequency(unsigned int freq)
{
	// 250 => ~30min, 125 => ~1h, 50 => ~2h30
	unsigned int meanTimeInSec = (250/freq) * 30;
	
	int deviation = 0;

	if(RANDOMIZEDRATIO > 0 && RANDOMIZEDRATIO < 100)
	{
		unsigned int maxDeviation = (meanTimeInSec * 2 * RANDOMIZEDRATIO) / 100;
		if(maxDeviation > 0)
		{
			deviation = qrand() % (maxDeviation);
		}
		deviation -= (maxDeviation/2);
	}

	return meanTimeInSec + deviation;
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
		SendSurprise(b);
	}
	// Restart Timer
	createCron(b);
}

void PluginSurprise::SendSurprise(Bunny * b)
{
	QByteArray file;
	// Fetch available files
	QDir * dir = GetLocalHTTPFolder();
	if(dir)
	{
		QString surprise = b->GetPluginSetting(GetName(), "folder", QString()).toString();

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

/*******
 * API *
 *******/

void PluginSurprise::InitApiCalls()
{
	DECLARE_PLUGIN_BUNNY_API_CALL("setFolder(name)", PluginSurprise, Api_SetFolder);
	DECLARE_PLUGIN_BUNNY_API_CALL("getFolder()", PluginSurprise, Api_GetFolder);
	DECLARE_PLUGIN_BUNNY_API_CALL("getFolderList()", PluginSurprise, Api_GetFolderList);
	DECLARE_PLUGIN_BUNNY_API_CALL("setFrequency(value)", PluginSurprise, Api_SetFrequency);
	DECLARE_PLUGIN_BUNNY_API_CALL("getFrequency()", PluginSurprise, Api_GetFrequency);
}

PLUGIN_BUNNY_API_CALL(PluginSurprise::Api_SetFolder)
{
	Q_UNUSED(account);

	QString folder = hRequest.GetArg("name");
	if(availableSurprises.contains(folder))
	{
		// Save new config
		bunny->SetPluginSetting(GetName(), "folder", folder);

		return new ApiManager::ApiOk(QString("Folder changed to '%1'").arg(folder));
	}
	return new ApiManager::ApiError(QString("Unknown '%1' folder").arg(folder));
}

PLUGIN_BUNNY_API_CALL(PluginSurprise::Api_GetFolder)
{
        Q_UNUSED(account);
	Q_UNUSED(hRequest);

        return new ApiManager::ApiOk(bunny->GetPluginSetting(GetName(), "folder", QString()).toString());
}

PLUGIN_BUNNY_API_CALL(PluginSurprise::Api_SetFrequency)
{
	Q_UNUSED(account);

	bunny->SetPluginSetting(GetName(), "frequency", QVariant(hRequest.GetArg("value").toInt()));
	OnBunnyDisconnect(bunny);
	OnBunnyConnect(bunny);
	return new ApiManager::ApiOk(QString("Plugin configuration updated."));
}

PLUGIN_BUNNY_API_CALL(PluginSurprise::Api_GetFrequency)
{
        Q_UNUSED(account);
	Q_UNUSED(hRequest);

        return new ApiManager::ApiOk(QString::number(bunny->GetPluginSetting(GetName(), "frequency", (uint)0).toInt()));
}

PLUGIN_BUNNY_API_CALL(PluginSurprise::Api_GetFolderList)
{
	Q_UNUSED(account);
	Q_UNUSED(bunny);
	Q_UNUSED(hRequest);

	// Check available folders and cache them
	QDir * httpFolder = GetLocalHTTPFolder();
	if(httpFolder)
	{
		availableSurprises = httpFolder->entryList(QDir::Dirs|QDir::NoDotAndDotDot);
		delete httpFolder;
	}

	return new ApiManager::ApiList(availableSurprises);
}
