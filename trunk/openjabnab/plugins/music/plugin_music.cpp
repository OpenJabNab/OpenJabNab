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
	Log::Info(QString("Will now play : %1").arg(music));

	QByteArray message = "ST "+GetBroadcastHTTPPath(music)+"\nPL "+QString::number(qrand() % 8).toAscii()+"\nMW\n";
	b->SendPacket(MessagePacket(message));
}
