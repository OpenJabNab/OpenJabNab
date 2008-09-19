#include <QDir>
#include <QCryptographicHash>
#include "plugin_tts.h"
#include "bunny.h"
#include "bunnymanager.h"
#include "httprequest.h"
#include "messagepacket.h"
#include "settings.h"
#include "ttsmanager.h"

Q_EXPORT_PLUGIN2(plugin_tts, PluginTTS)

PluginTTS::PluginTTS():PluginInterface("tts")
{
}

ApiManager::ApiAnswer * PluginTTS::ProcessApiCall(QByteArray const& funcName, HTTPRequest const& r)
{
	if(funcName.toLower() != "say")
		return new ApiManager::ApiError(QString("Bad function name for plugin TTS"));
	if(!r.HasArg("to"))
		return new ApiManager::ApiError(QString("Missing argument 'to' for plugin TTS"));
	if(!r.HasArg("text"))
		return new ApiManager::ApiError(QString("Missing argument 'text' for plugin TTS"));

	Bunny * b = BunnyManager::GetConnectedBunny(r.GetArg("to").toAscii());
	if(!b)
		return new ApiManager::ApiError(QString("Bunny '%1' is not connected").arg(r.GetArg("to")));

	QDir ttsFolder(GlobalSettings::GetString("Config/HttpPath"));
	if (!ttsFolder.cd("tts"))
	{
		if (!ttsFolder.mkdir("tts"))
		{
			Log::Error("Unable to create plugin/tts directory !\n");
			return new ApiManager::ApiError(QString("Internal error with plugin, please check logs"));
		}
		ttsFolder.cd("tts");
	}
	QByteArray fileName = QCryptographicHash::hash(r.GetArg("text").toAscii(), QCryptographicHash::Md5).toHex().append(".mp3");
	if(!QFile::exists(ttsFolder.absoluteFilePath(fileName)))
	{
		TTSManager * tts = new TTSManager();
		tts->createNewSound(r.GetArg("text"), "claire", QString("tts/").append(fileName));
	}
	b->SendPacket(MessagePacket("MU broadcast/ojn_local/tts/" + fileName + "\nMW\n"));
	return new ApiManager::ApiString(QString("Sending '%1' to bunny '%2'").arg(r.GetArg("text"), r.GetArg("to")));
}
