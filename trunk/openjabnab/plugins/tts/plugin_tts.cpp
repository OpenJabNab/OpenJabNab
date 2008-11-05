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

ApiManager::ApiAnswer * PluginTTS::ProcessBunnyApiCall(Bunny * b, Account const&, QString const& funcName, HTTPRequest const& r)
{
	if(funcName.toLower() != "say")
		return new ApiManager::ApiError(QString("Bad function name for plugin TTS"));
	if(!r.HasArg("text"))
		return new ApiManager::ApiError(QString("Missing argument 'text' for plugin TTS"));

	if(!b->IsConnected())
		return new ApiManager::ApiError(QString("Bunny '%1' is not connected").arg(QString(b->GetID())));

	std::auto_ptr<QDir> ttsFolder(GetLocalHTTPFolder());
	if(!ttsFolder.get())
			return new ApiManager::ApiError(QString("Can't create output folder, please check logs"));

	QByteArray fileName = QCryptographicHash::hash(r.GetArg("text").toAscii(), QCryptographicHash::Md5).toHex().append(".mp3");
	QString filePath = ttsFolder->absoluteFilePath(fileName);
	if(!QFile::exists(filePath))
		TTSManager::CreateNewSound(r.GetArg("text"), "claire", filePath);

	b->SendPacket(MessagePacket("MU " + GetBroadcastHTTPPath(fileName) + "\nMW\n"));
	return new ApiManager::ApiOk(QString("Sending '%1' to bunny '%2'").arg(r.GetArg("text"), QString(b->GetID())));
}
