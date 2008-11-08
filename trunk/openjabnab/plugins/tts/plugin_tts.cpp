#include <QDir>
#include <QCryptographicHash>
#include <memory>
#include "plugin_tts.h"
#include "bunny.h"
#include "bunnymanager.h"
#include "httprequest.h"
#include "messagepacket.h"
#include "settings.h"
#include "ttsmanager.h"

Q_EXPORT_PLUGIN2(plugin_tts, PluginTTS)

PluginTTS::PluginTTS():PluginInterface("tts", "TTS Plugin, Send Text to Bunny")
{
}

void PluginTTS::InitApiCalls()
{
	DECLARE_PLUGIN_BUNNY_API_CALL("say", PluginTTS, Api_Say);
}

PLUGIN_BUNNY_API_CALL(PluginTTS::Api_Say)
{
	Q_UNUSED(account);

	if(!hRequest.HasArg("text"))
		return new ApiManager::ApiError(QString("Missing argument 'text' for plugin TTS"));

	if(!bunny->IsConnected())
		return new ApiManager::ApiError(QString("Bunny '%1' is not connected").arg(QString(bunny->GetID())));

	std::auto_ptr<QDir> ttsFolder(GetLocalHTTPFolder());
	if(!ttsFolder.get())
			return new ApiManager::ApiError(QString("Can't create output folder, please check logs"));

	QByteArray fileName = QCryptographicHash::hash(hRequest.GetArg("text").toAscii(), QCryptographicHash::Md5).toHex().append(".mp3");
	QString filePath = ttsFolder->absoluteFilePath(fileName);
	if(!QFile::exists(filePath))
		TTSManager::CreateNewSound(hRequest.GetArg("text"), "claire", filePath);

	bunny->SendPacket(MessagePacket("MU " + GetBroadcastHTTPPath(fileName) + "\nMW\n"));
	return new ApiManager::ApiOk(QString("Sending '%1' to bunny '%2'").arg(hRequest.GetArg("text"), QString(bunny->GetID())));
}
