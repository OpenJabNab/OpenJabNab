#include <QDataStream>
#include <QFile>
#include <QHttp>
#include <QHttpRequestHeader>
#include <QStringList>
#include <QUrl>
#include "log.h"
#include "settings.h"
#include "soundmanager.h"

bool SoundManager::createNewSound(QString text, QString voice, QString fileName)
{
	QStringList voiceList;
	// French voices
	voiceList << "claire" << "alice" << "bruno" << "julie";

	if(!voiceList.contains(voice))
		voice = "claire";

	QHttp http("vaas3.acapela-group.com");
	connect(&http, SIGNAL(done(bool)), this, SLOT(downloadDone()));

	QByteArray ContentData;
	ContentData += "php%5Fvar%5Fdec=undefined&php%5Fvar%5Fhtml=undefined&php%5Fvar%5Fstring=" + voice + "22k%5B%2Fvoix%5D" + QUrl::toPercentEncoding(text) + "&php%5Fvar%5Fnom=whatever%2Etxt&php%5Fverif=flash&onLoad=%5Btype%20Function%5D";

	QHttpRequestHeader Header;
	Header.addValue("Host", "vaas3.acapela-group.com");
	Header.addValue("User-Agent", "Mozilla/5.0 (X11; U; Linux i686; fr; rv:1.9.0.1) Gecko/2008072820 Firefox/3.0.1");
	Header.addValue("Referer", "http://vaas3.acapela-group.com/Demo_Web/synthese_movieclip_small.swf");
	Header.addValue("Content-type", "application/x-www-form-urlencoded");
	Header.addValue("Content-length", QString::number(ContentData.length()));
	Header.setRequest("POST", "/Demo_Web/write.php", 1, 1);

	http.request(Header, ContentData);
	loop.exec();
	QByteArray reponse = http.readAll();
	if(reponse.startsWith("&retour_php"))
	{
		QString acapelaFile = reponse.mid(12);
		http.get("/Demo_Web/Sorties/" + acapelaFile + ".mp3");
		loop.exec();
		QFile file(GlobalSettings::GetString("OpenJabNabServers/HttpPath") + fileName);
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
	return false;
}

void SoundManager::downloadDone()
{
	loop.exit();
}
