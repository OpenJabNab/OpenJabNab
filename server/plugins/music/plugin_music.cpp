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
#include "log.h"
#include "cron.h"
#include "messagepacket.h"
#include "plugin_music.h"
#include "settings.h"
#include "ttsmanager.h"

Q_EXPORT_PLUGIN2(plugin_music, PluginMusic)

PluginMusic::PluginMusic():PluginInterface("music", "Music", BunnyZtampPlugin) {}

bool PluginMusic::Init()
{
	std::auto_ptr<QDir> dir(GetLocalHTTPFolder());
	if(dir.get())
	{
		QStringList filters;
		filters << "*.mp3";
		musicFolder = *dir;
		musicFolder.setNameFilters(filters);
		return true;
	}
	return false;
}

bool PluginMusic::OnRFID(Ztamp * z, Bunny * b)
{
	QString music = z->GetPluginSetting(GetName(), QString("Play"), QString()).toString();
	if(music != "")
	{
		LogInfo(QString("Will now play from ztamp (user choice) : %1").arg(music));

		QByteArray message = "ST "+GetBroadcastHTTPPath(music)+"\nPL "+QString::number(qrand() % 8).toAscii()+"\nMW\n";
		b->SendPacket(MessagePacket(message));
		return true;
	}
	return false;
}

bool PluginMusic::OnRFID(Bunny * b, QByteArray const& tag)
{
	QString music = b->GetPluginSetting(GetName(), QString("RFIDPlay/%1").arg(QString(tag.toHex())), QString()).toString();
	if(music != "")
	{
		LogInfo(QString("Will now play (user choice) : %1").arg(music));

		QByteArray message = "ST "+GetBroadcastHTTPPath(music)+"\nPL "+QString::number(qrand() % 8).toAscii()+"\nMW\n";
		b->SendPacket(MessagePacket(message));
		return true;
	}
	return false;
}

bool PluginMusic::OnClick(Bunny * b, PluginInterface::ClickType type)
{
	if (type == PluginInterface::SingleClick)
	{
		getMusicList(b);
		return true;
	}
	return false;
}

void PluginMusic::getMusicList(Bunny * b)
{
	if(!b->IsIdle())
	{
		LogError("PluginMusic::getMusicList but bunny is not idle");
	} else {
		QStringList musics = musicFolder.entryList();
		int index = 0;
		if(musics.count() > 1)
		{
			index = qrand() % musics.count();
			QString music = musics.at(index);
			LogInfo(QString("Will now play (random) : %1").arg(music));

			QByteArray message = "ST "+GetBroadcastHTTPPath(music)+"\nPL "+QString::number(qrand() % 8).toAscii()+"\nMW\n";
			b->SendPacket(MessagePacket(message));
		}

	}
}

/*******
 * API *
 *******/

void PluginMusic::InitApiCalls()
{
	DECLARE_PLUGIN_BUNNY_API_CALL("addrfid(tag,music)", PluginMusic, Api_AddRFID);
	DECLARE_PLUGIN_BUNNY_API_CALL("removerfid(tag)", PluginMusic, Api_RemoveRFID);
	DECLARE_PLUGIN_BUNNY_API_CALL("getfileslist()", PluginMusic, Api_getFilesList);
}

PLUGIN_BUNNY_API_CALL(PluginMusic::Api_AddRFID)
{
	Q_UNUSED(account);

	bunny->SetPluginSetting(GetName(), QString("RFIDPlay/%1").arg(hRequest.GetArg("tag")), hRequest.GetArg("music"));

	return new ApiManager::ApiOk(QString("Add '%1' for RFID '%2', bunny '%3'").arg(hRequest.GetArg("music"), hRequest.GetArg("tag"), QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginMusic::Api_RemoveRFID)
{
	Q_UNUSED(account);

	bunny->RemovePluginSetting(GetName(), QString("RFIDPlay/%1").arg(hRequest.GetArg("tag")));

	return new ApiManager::ApiOk(QString("Remove RFID '%2' for bunny '%3'").arg(hRequest.GetArg("tag"), QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginMusic::Api_getFilesList)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);
	Q_UNUSED(bunny);

	return new ApiManager::ApiList(musicFolder.entryList());
}
