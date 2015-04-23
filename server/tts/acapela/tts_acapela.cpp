#include <QDateTime>
#include <QUrl>
#include <QCryptographicHash>
#include <QMapIterator>
#include "tts_acapela.h"
#include "log.h"
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>

Q_EXPORT_PLUGIN2(tts_acapela, TTSAcapela)

TTSAcapela::TTSAcapela():TTSInterface("acapela", "Acapela")
{
	// Arabic
	voiceList.insert("Leila", "sonid0");
	voiceList.insert("Mehdi", "sonid0");
	voiceList.insert("Nizar", "sonid0");
	voiceList.insert("Salma", "sonid0");
	// English (AU)
	voiceList.insert("Liam", "sonid6");
	voiceList.insert("Lisa", "sonid6");
	voiceList.insert("Olivia", "sonid6");
	voiceList.insert("Tyler", "sonid6");
	// English (India)
	voiceList.insert("Deepa", "sonid7");
	// English (UK)
	voiceList.insert("Graham", "sonid8");
	voiceList.insert("Harry", "sonid8");
	voiceList.insert("Lucy", "sonid8");
	voiceList.insert("Nizareng", "sonid8");
	voiceList.insert("Peter", "sonid8");
	voiceList.insert("PeterHappy", "sonid8");
	voiceList.insert("PeterSad", "sonid8");
	voiceList.insert("QueenElizabeth", "sonid8");
	voiceList.insert("Rachel", "sonid8");
	voiceList.insert("Rosie", "sonid8");
	// French (Belgium)
	voiceList.insert("Justine", "sonid11");
	// French (Canada)
	voiceList.insert("Louise", "sonid12");
	// French (France)
	voiceList.insert("Alice", "sonid13");
	voiceList.insert("Antoine", "sonid13");
	voiceList.insert("AntoineFromAfar", "sonid13");
	voiceList.insert("AntoineHappy", "sonid13");
	voiceList.insert("AntoineSad", "sonid13");
	voiceList.insert("AntoineUpClose", "sonid13");
	voiceList.insert("Bruno", "sonid13");
	voiceList.insert("Claire", "sonid13");
	voiceList.insert("Julie", "sonid13");
	voiceList.insert("Manon", "sonid13");
	voiceList.insert("Margaux", "sonid13");
	voiceList.insert("MargauxHappy", "sonid13");
	voiceList.insert("MargauxSad", "sonid13");
	// German
	voiceList.insert("Andreas", "sonid14");
	voiceList.insert("Jonas", "sonid14");
	voiceList.insert("Julia", "sonid14");
	voiceList.insert("Klaus", "sonid14");
	voiceList.insert("Lea", "sonid14");
	voiceList.insert("Sarah", "sonid14");
	// Italian
	voiceList.insert("chiara", "sonid16");
	voiceList.insert("fabiana", "sonid16");
	voiceList.insert("vittorio", "sonid16");
	// Russian
	voiceList.insert("Alyona", "sonid24");
	// Spanish (Spain)
	voiceList.insert("Antonio", "sonid25");
	voiceList.insert("Ines", "sonid25");
	voiceList.insert("Maria", "sonid25");
	// Spanish (US)
	voiceList.insert("Rodrigo", "sonid26");
	voiceList.insert("Rosa", "sonid26");
}

TTSAcapela::~TTSAcapela()
{
}

QByteArray TTSAcapela::CreateNewSound(QString text, QString voice, bool forceOverwrite)
{
	QEventLoop loop;

	if(!voiceList.contains(voice))
		voice = "Claire";

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
	QHttp http("www.acapela-group.com");
	QObject::connect(&http, SIGNAL(done(bool)), &loop, SLOT(quit()));

	QString langId = voiceList.value(voice);
	QString langName = voice;
	QByteArray ContentData;
	ContentData += "MyLanguages="+langId+"&0=Leila&1=Laia&2=Eliska&3=Mette&4=Jeroen&5=Daan&6=Liam&7=Deepa&8=Graham&9=Heather&10=Sanna&11=Justine&12=Louise&MySelectedVoice="+langName+"&14=Andreas&15=Dimitris&16=chiara&17=Sakura&18=Minji&19=Lulu&20=Bente&21=Ania&22=Marcia&23=Celia&24=Alyona&25=Antonio&26=Rodrigo&27=Elin&28=Samuel&29=Kal&30=Mia&31=Ipek&MyTextForTTS="+QUrl::toPercentEncoding(text)+"&t=1&SendToVaaS=";

	QHttpRequestHeader Header;
	Header.addValue("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
	Header.addValue("Accept-Encoding", "gzip, deflate");
	Header.addValue("Accept-Language", "fr,fr-fr;q=0.8,en-us;q=0.5,en;q=0.3");
	Header.addValue("Connection", "keep-alive");
	Header.addValue("Host", "www.acapela-group.com");
	Header.addValue("Referer", "http://www.acapela-group.com/demo-tts/DemoHTML5Form_V2.php?langdemo=Powered+by+%3Ca+href%3D%22http%3A%2F%2Fwww.acapela-vaas.com%22%3EAcapela+Voice+as+a+Service%3C%2Fa%3E.+For+demo+and+evaluation+purpose+only%2C+for+commercial+use+of+generated+sound+files+please+go+to+%3Ca+href%3D%22http%3A%2F%2Fwww.acapela-box.com%22%3Ewww.acapela-box.com%3C%2Fa%3E");
	Header.addValue("User-Agent", "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:28.0) Gecko/20100101 Firefox/28.0");
	Header.addValue("Content-Type", "application/x-www-form-urlencoded");

	Header.setContentLength(ContentData.length());
	Header.setRequest("POST", "/demo-tts/DemoHTML5Form_V2_fr.php?langdemo=Vocalis%C3%A9+par+%3Ca+href%3D%22http%3A%2F%2Fwww.acapela-vaas.com%22%3EAcapela+Voice+as+a+Service%3C%2Fa%3E.+Pour+d%C3%A9monstration+et+%C3%A9valuation+uniquement%2C+pour+toute+utilisation+commerciale+des+fichiers+sons+g%C3%A9n%C3%A9r%C3%A9s%2C+consultez+%3Ca+href%3D%22http%3A%2F%2Fwww.acapela-box.com%22%3Ewww.acapela-box.com%3C%2Fa%3E", 1, 1);
	http.request(Header, ContentData);
	loop.exec();
	QByteArray reponse = http.readAll();

	QRegExp rx("myPhpVar = '(http://[^']*mp3)';");
	rx.setMinimal(true);
	int pos = 0;
	if((pos = rx.indexIn(reponse, pos)) != -1 )
        {
                QString acapelaFile = rx.cap(1);
                QUrl urlfile(acapelaFile);
                QNetworkAccessManager * manager = new QNetworkAccessManager(this);
                QObject::connect(manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
                QNetworkReply * reply = manager->get(QNetworkRequest(urlfile));
                loop.exec();
                QFile file(filePath);
                if (!file.open(QIODevice::WriteOnly))
                {
                    LogError("Cannot open sound file for writing");
                    return QByteArray();
                }
                file.write(reply->readAll());
                file.close();
                return ttsHTTPUrl.arg(voice, fileName).toAscii();
        }
 	LogError("Acapela demo did not return a sound file");
	LogDebug(QString("Acapela answer : %1").arg(QString(reponse)));
	return QByteArray();

}

