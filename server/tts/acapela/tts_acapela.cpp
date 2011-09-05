#include <QDateTime>
#include <QUrl>
#include <QCryptographicHash>
#include <QMapIterator>
#include "tts_acapela.h"
#include "log.h"

Q_EXPORT_PLUGIN2(tts_acapela, TTSAcapela)

TTSAcapela::TTSAcapela():TTSInterface("acapela", "Acapela")
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
}

TTSAcapela::~TTSAcapela()
{
}

QByteArray TTSAcapela::CreateNewSound(QString text, QString voice, bool forceOverwrite)
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
	QHttp http("vaas.acapela-group.com");
	QObject::connect(&http, SIGNAL(done(bool)), &loop, SLOT(quit()));

	QByteArray ContentData;
	ContentData += "client%5Ftext=" + QUrl::toPercentEncoding(text) + "&client%5Fvoice=" + voice + "22k&client%5Frequest%5Ftype=CREATE%5FREQUEST&client%5Fpassword=demo%5Fweb&client%5Flogin=asTTS&client%5Fversion=1%2D00&actionscript%5Fversion=3";

	QHttpRequestHeader Header;
	Header.addValue("Host", "vaas.acapela-group.com");
	Header.addValue("User-Agent", "Mozilla/5.0 (X11; U; Linux i686; fr; rv:1.9.0.1) Gecko/2011000000 Firefox/6.0.1");
	Header.addValue("Referer", "http://www.acapela-group.com/Flash/Demo_Web_AS3/demo_web.swf?path=http://vaas.acapela-group.com/Services/DemoWeb/&lang=EN");
	Header.addValue("Content-type", "application/x-www-form-urlencoded");
	Header.addValue("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
	Header.addValue("Accept-Language", "fr,fr-fr;q=0.8,en-us;q=0.5,en;q=0.3");
	Header.addValue("Accept-Encoding", "gzip,deflate");
	Header.addValue("Accept-Charset", "ISO-8859-1,utf-8;q=0.7,*;q=0.7");
	Header.addValue("Keep-Alive", "300");
	Header.addValue("Connection", "keep-alive");

	Header.setContentLength(ContentData.length());
	Header.setRequest("POST", "/Services/DemoWeb/textToMP3.php", 1, 1);
	http.request(Header, ContentData);
	loop.exec();
	QByteArray reponse = http.readAll();
	QUrl url("?"+reponse);
	if(reponse.startsWith("retour_php"))
	{
		//LogDebug(QString("Acapela answer : %1").arg(QString(reponse)));
		QString acapelaFile = url.queryItemValue("retour_php");
		QUrl urlfile(acapelaFile);
		http.setHost(urlfile.host());
		Header.setRequest("GET",urlfile.path(),1,1);
		//LogInfo(QString("Downloading MP3 file : %1").arg(QString(acapelaFile)));
		http.request(Header,"");
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

