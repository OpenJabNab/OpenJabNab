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

PluginMusic::PluginMusic():PluginInterface("music", "Music") {}

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

bool PluginMusic::OnRFID(Bunny * b, QByteArray const& tag)
{
	QString music = b->GetPluginSetting(GetName(), QString("RFIDPlay/%1").arg(QString(tag)), QString()).toString();
	if(music != "")
	{
		Log::Info(QString("Will now play (user choice) : %1").arg(music));

		QByteArray message = "ST "+GetBroadcastHTTPPath(music)+"\nPL "+QString::number(qrand() % 8).toAscii()+"\nMW\n";
		b->SendPacket(MessagePacket(message));
		return true;
	}
	return false;
}

bool PluginMusic::OnClick(Bunny * b, PluginInterface::ClickType type)
{
	if (type == PluginInterface::SingleClick && b->GetGlobalSetting("singleClickPlugin", "").toByteArray() == GetName())
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
		Log::Error("PluginMusic::getMusicList but bunny is not idle");
		return;
	}
	
	QStringList musics = musicFolder.entryList();
	int index = 0;
	if(musics.count() > 1)
	{
		index = qrand() % musics.count();
	}
	QString music = musics.at(index);
	Log::Info(QString("Will now play (random) : %1").arg(music));

	QByteArray message = "ST "+GetBroadcastHTTPPath(music)+"\nPL "+QString::number(qrand() % 8).toAscii()+"\nMW\n";
	b->SendPacket(MessagePacket(message));
}

void PluginMusic::InitApiCalls()
{
	DECLARE_PLUGIN_BUNNY_API_CALL("addrfid", PluginMusic, Api_AddRFID);
	DECLARE_PLUGIN_BUNNY_API_CALL("removerfid", PluginMusic, Api_RemoveRFID);
}

PLUGIN_BUNNY_API_CALL(PluginMusic::Api_AddRFID)
{
	Q_UNUSED(account);

	if(!hRequest.HasArg("music"))
		return new ApiManager::ApiError(QString("Missing argument 'music' for plugin Music"));

	if(!hRequest.HasArg("tag"))
		return new ApiManager::ApiError(QString("Missing argument 'tag' for plugin Music"));

	if(!bunny->IsConnected())
		return new ApiManager::ApiError(QString("Bunny '%1' is not connected").arg(hRequest.GetArg("to")));

	bunny->SetPluginSetting(GetName(), QString("RFIDPlay/%1").arg(hRequest.GetArg("tag")), hRequest.GetArg("music"));

	return new ApiManager::ApiString(QString("Add '%1' for RFID '%2', bunny '%3'").arg(hRequest.GetArg("music"), hRequest.GetArg("tag"), QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginMusic::Api_RemoveRFID)
{
	Q_UNUSED(account);

	if(!hRequest.HasArg("tag"))
		return new ApiManager::ApiError(QString("Missing argument 'tag' for plugin Music"));

	if(!bunny->IsConnected())
		return new ApiManager::ApiError(QString("Bunny '%1' is not connected").arg(hRequest.GetArg("to")));

	bunny->RemovePluginSetting(GetName(), QString("RFIDPlay/%1").arg(hRequest.GetArg("tag")));

	return new ApiManager::ApiString(QString("Remove RFID '%2' for bunny '%3'").arg(hRequest.GetArg("tag"), QString(bunny->GetID())));
}

