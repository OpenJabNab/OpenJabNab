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

bool TTSManager::CreateNewSound(QString text, QString voice, QString fileName, bool forceOverwrite)
{
	QStringList voiceList;
	QEventLoop loop;
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

	if(!forceOverwrite && QFile::exists(fileName))
		return true;

	if(!voiceList.contains(voice))
		voice = "claire";

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
		Log::Debug(QString("Acapela answer : %1").arg(QString(reponse)));
		QString acapelaFile = url.queryItemValue("retour_php");
		Log::Info(QString("Downloading MP3 file : %1").arg(QString(acapelaFile)));
		http.get(acapelaFile);
		loop.exec();
		QFile file(fileName);
		if (!file.open(QIODevice::WriteOnly))
		{
			Log::Error("Cannot open sound file for writing");
			return false;
		}
		file.write(http.readAll());
		file.close();
		return true;
	}
	Log::Error("Acapela demo did not return a sound file");
	Log::Debug(QString("Acapela answer : %1").arg(QString(reponse)));
	return false;
}
