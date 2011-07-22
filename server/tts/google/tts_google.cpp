#include <QDateTime>
#include <QUrl>
#include <QCryptographicHash>
#include <QMapIterator>
#include "tts_google.h"
#include "log.h"

Q_EXPORT_PLUGIN2(tts_google, TTSGoogle)

TTSGoogle::TTSGoogle():TTSInterface("google", "Google")
{
	voiceList << "fr";
}

TTSGoogle::~TTSGoogle()
{
}

QByteArray TTSGoogle::CreateNewSound(QString text, QString voice, bool forceOverwrite)
{
	QEventLoop loop;

	if(!voiceList.contains(voice))
		voice = "fr";

	// Check (and create if needed) output folder
	QDir outputFolder = ttsFolder;
	if(!outputFolder.exists(voice))
		outputFolder.mkdir(voice);
	
	if(!outputFolder.cd(voice))
	{
		LogError(QString("Cant create TTS Folder : %1").arg(ttsFolder.absoluteFilePath(voice)));
		return QByteArray();
	}
	
	// Compute fileName
	QString fileName = QCryptographicHash::hash(text.toAscii(), QCryptographicHash::Md5).toHex().append(".mp3");
	QString filePath = outputFolder.absoluteFilePath(fileName);

	if(!forceOverwrite && QFile::exists(filePath))
		return ttsHTTPUrl.arg(voice, fileName).toAscii();

	// Fetch MP3
	QHttp http("translate.google.com");
	QObject::connect(&http, SIGNAL(done(bool)), &loop, SLOT(quit()));

	QByteArray ContentData;
	ContentData += "ie=UTF-8&q=" + QUrl::toPercentEncoding(text) + "&tl=" + voice;

	QHttpRequestHeader Header;
	Header.addValue("Host", "translate.google.com");

	Header.setContentLength(ContentData.length());
	Header.setRequest("POST", "/translate_tts", 1, 1);

	http.request(Header, ContentData);
	loop.exec();

	QFile file(filePath);
	if (!file.open(QIODevice::WriteOnly))
	{
		LogError("Cannot open sound file for writing");
		return QByteArray();
	}
	file.write(http.readAll());
	file.close();
	return ttsHTTPUrl.arg(voice, fileName).toAscii();
}

