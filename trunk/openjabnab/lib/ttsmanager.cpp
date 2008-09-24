#include <QDataStream>
#include <QFile>
#include <QHttp>
#include <QHttpRequestHeader>
#include <QStringList>
#include <QUrl>
#include "log.h"
#include "settings.h"
#include "ttsmanager.h"

bool TTSManager::createNewSound(QString text, QString voice, QString fileName)
{
	QStringList voiceList;
	// French voices
	voiceList << "claire" << "alice" << "bruno" << "julie";

	if(!voiceList.contains(voice))
		voice = "claire";

	QHttp http("vaas3.acapela-group.com");
	connect(&http, SIGNAL(done(bool)), this, SLOT(downloadDone()));

	QByteArray ContentData;
//	ContentData += "php%5Fvar%5Fdec=undefined&php%5Fvar%5Fhtml=undefined&php%5Fvar%5Fstring=" + voice + "22k%5B%2Fvoix%5D" + QUrl::toPercentEncoding(text) + "&php%5Fvar%5Fnom=whatever%2Etxt&php%5Fverif=flash&onLoad=%5Btype%20Function%5D";
	ContentData += "client%5Ftext=" + QUrl::toPercentEncoding(text) + "&client%5Fvoice=" + voice + "22k&client%5Frequest%5Ftype=CREATE%5FREQUEST&client%5Fpassword=reuno&client%5Flogin=asTTS&client%5Fversion=1%2D00&onLoad=%5Btype%20Function%5D";

	QHttpRequestHeader Header;
	Header.addValue("Host", "vaas3.acapela-group.com");
	Header.addValue("User-Agent", "Mozilla/5.0 (X11; U; Linux i686; fr; rv:1.9.0.1) Gecko/2008072820 Firefox/3.0.1");
	Header.addValue("Referer", "http://vaas3.acapela-group.com/Demo_Web/synthese_movieclip_small.swf");
	Header.addValue("Content-type", "application/x-www-form-urlencoded");
	Header.addValue("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
	Header.addValue("Accept-Language", "fr,fr-fr;q=0.8,en-us;q=0.5,en;q=0.3");
	Header.addValue("Accept-Encoding", "gzip,deflate");
	Header.addValue("Accept-Charset", "ISO-8859-1,utf-8;q=0.7,*;q=0.7");
	Header.addValue("Keep-Alive", "300");
	Header.addValue("Connection", "keep-alive");

	Header.setContentLength(ContentData.length());
	Header.setRequest("POST", "/asTTS/v1-00/textToMP3.php", 1, 1);

	http.request(Header, ContentData);
	loop.exec();
	QByteArray reponse = http.readAll();
	if(reponse.startsWith("retour_php"))
	{
		QString acapelaFile = reponse.mid(44, 13);
		Log::Debug("Downloading MP3 file : " + acapelaFile);
		http.get("/asTTS/v1-00/sounds/" + acapelaFile + ".mp3");
		loop.exec();
		QFile file(GlobalSettings::GetString("Config/HttpPath") + fileName);
		if (!file.open(QIODevice::WriteOnly))
		{
			Log::Error("Cannot open config file for writing");
			return false;
		}
		file.write(http.readAll());
		file.close();
		return true;
	}
	Log::Error("Acapela demo did not return a sound file");
	Log::Debug("Acapela answer : " + reponse);
	return false;
}

void TTSManager::downloadDone()
{
	loop.exit();
}
