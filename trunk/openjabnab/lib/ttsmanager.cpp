#include <QCryptographicHash>
#include <QDataStream>
#include <QEventLoop>
#include <QFile>
#include <QHttp>
#include <QHttpRequestHeader>
#include <QObject>
#include <QStringList>
#include <QUrl>
#include "log.h"
#include "settings.h"
#include "ttsmanager.h"

QStringList TTSManager::voiceList;
QDir TTSManager::ttsFolder;
QString TTSManager::ttsHTTPUrl;

#define TTS_FOLDER "tts"

void TTSManager::Init()
{
	// French voices
	voiceList << "claire" << "alice" << "bruno" << "julie";
	// Canadian French voices
	voiceList << "louise";
	// German voices
	voiceList << "klaus" << "sarah";
	// Spanish voices
	voiceList << "maria";
	// UK English voices
	voiceList << "graham" << "lucy" << "peter" << "rachel";
	// US English voices
	voiceList << "heather" << "kenny" << "laura" << "nelly" << "ryan";
	
	// Folder
	QDir folder(GlobalSettings::GetString("Config/RealHttpRoot"));
	// Try to create tts subfolder
	if (!folder.cd(TTS_FOLDER))
	{
		if (!folder.mkdir(TTS_FOLDER))
		{
			LogError(QString("Unable to create "TTS_FOLDER" directory !\n"));
			return;
		}
		folder.cd(TTS_FOLDER);
	}
	ttsFolder = folder;
	ttsHTTPUrl = "broadcast/ojn_local/"TTS_FOLDER"/%1/%2"; // %1 For voice, %2 for FileName
}

// Creatte TTS Song in /broadcast/tts/voice/[md5].mp3
QByteArray TTSManager::CreateNewSound(QString text, QString voice, bool forceOverwrite)
{
	QEventLoop loop;

	if(!voiceList.contains(voice))
		voice = "claire";

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
	QHttp http("vaas3.acapela-group.com");
	QObject::connect(&http, SIGNAL(done(bool)), &loop, SLOT(quit()));

	QByteArray ContentData;
	ContentData += "client%5Ftext=" + QUrl::toPercentEncoding(text) + "&client%5Fvoice=" + voice + "22k&client%5Frequest%5Ftype=CREATE%5FREQUEST&client%5Fpassword=demo_web&client%5Flogin=asTTS&client%5Fversion=1%2D00&actionscript%5Fversion=3";

	QHttpRequestHeader Header;
	Header.addValue("Host", "vaas3.acapela-group.com");
	Header.addValue("User-Agent", "Mozilla/5.0 (X11; U; Linux i686; fr; rv:1.9.0.1) Gecko/2008072820 Firefox/3.0.1");
	Header.addValue("Referer", "http://www.acapela-group.com/Flash/Demo_Web_AS3/demo_web.swf?path=http://vaas3.acapela-group.com/connector/1-20/&lang=FR");
	Header.addValue("Content-type", "application/x-www-form-urlencoded");
	Header.addValue("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
	Header.addValue("Accept-Language", "fr,fr-fr;q=0.8,en-us;q=0.5,en;q=0.3");
	Header.addValue("Accept-Encoding", "gzip,deflate");
	Header.addValue("Accept-Charset", "ISO-8859-1,utf-8;q=0.7,*;q=0.7");
	Header.addValue("Keep-Alive", "300");
	Header.addValue("Connection", "keep-alive");

	Header.setContentLength(ContentData.length());
	Header.setRequest("POST", "/connector/1-20/textToMP3.php", 1, 1);

	http.request(Header, ContentData);
	loop.exec();
	QByteArray reponse = http.readAll();
	QUrl url("?"+reponse);
	if(url.hasQueryItem("retour_php"))
	{
		LogDebug(QString("Acapela answer : %1").arg(QString(reponse)));
		QString acapelaFile = url.queryItemValue("retour_php");
		LogInfo(QString("Downloading MP3 file : %1").arg(QString(acapelaFile)));
		http.get(acapelaFile);
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
	LogError("Acapela demo did not return a sound file");
	LogDebug(QString("Acapela answer : %1").arg(QString(reponse)));
	return QByteArray();
}