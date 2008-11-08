#include <QDateTime>
#include <QCryptographicHash>
#include <QXmlStreamReader>
#include <QHttp>
#include <QMapIterator>
#include <QRegExp>
#include <QUrl>
#include <memory>
#include "bunny.h"
#include "bunnymanager.h"
#include "httprequest.h"
#include "log.h"
#include "cron.h"
#include "messagepacket.h"
#include "plugin_tv.h"
#include "settings.h"
#include "ttsmanager.h"

Q_EXPORT_PLUGIN2(plugin_tv, PluginTV)

PluginTV::PluginTV():PluginInterface("tv", "Programme TV")
{
	std::auto_ptr<QDir> dir(GetLocalHTTPFolder());
	if(dir.get())
	{
		tvFolder = *dir;
		TTSManager::CreateNewSound("Programme télé de ce soir", "claire", tvFolder.absoluteFilePath("cesoir.mp3"));
	}
}

PluginTV::~PluginTV()
{
	Cron::UnregisterAll(this);
}

void PluginTV::OnCron(QVariant v)
{
	Bunny * b = QVariantHelper::ToBunnyPtr(v);
	getTVPage(b);
}

bool PluginTV::OnClick(Bunny * b, PluginInterface::ClickType type)
{
	if (type == PluginInterface::SingleClick)
	{
		getTVPage(b);
		return true;
	}
	return false;
}

void PluginTV::getTVPage(Bunny * b)
{
	QHttp* http = new QHttp(this);
	http->setProperty("BunnyID", b->GetID());
	connect(http, SIGNAL(done(bool)), this, SLOT(analyseXml()));
	http->setHost("www.programme-tv.com");
	http->get("/rss.xml");
}

void PluginTV::analyseXml()
{
	std::auto_ptr<QHttp> http(qobject_cast<QHttp *>(sender()));
	Bunny * bunny = BunnyManager::GetBunny(this, http->property("BunnyID").toByteArray());
	if(!bunny)
		return;

	PluginTV_Worker * p = new PluginTV_Worker(this, bunny, http->readAll());
	connect(p, SIGNAL(done(bool,Bunny*,QByteArray)), this, SLOT(analyseDone(bool,Bunny*,QByteArray)));
	connect(p, SIGNAL(finished()), p, SLOT(deleteLater()));
	p->start();
}

void PluginTV::analyseDone(bool ret, Bunny * b, QByteArray message)
{
	if(ret && b->IsConnected())
		 b->SendPacket(MessagePacket(message));
}

void PluginTV::OnBunnyConnect(Bunny * b)
{
	if(!webcastList.contains(b))
	{
		QStringList webcasts = b->GetPluginSetting(GetName(), "Webcast/List", QStringList()).toStringList();
		foreach(QString webcast, webcasts)
		{
			QStringList time = webcast.split(":");
			int id = Cron::Register(this, 60*24, time[0].toInt(), time[1].toInt(), QVariant::fromValue( b ));
			webcastList.insert(b , qMakePair(id, webcast));
		}
	}
}

void PluginTV::OnBunnyDisconnect(Bunny * b)
{
	typedef QPair<int, QString> listElement;
	QList<listElement> listOfWebcasts = webcastList.values(b);
	foreach(listElement l, listOfWebcasts)
	{
		Cron::Unregister(this, l.first);
	}
	webcastList.remove(b);
}

void PluginTV::InitApiCalls()
{
	DECLARE_PLUGIN_BUNNY_API_CALL("addwebcast", PluginTV, Api_AddWebcast);
	DECLARE_PLUGIN_BUNNY_API_CALL("removewebcast", PluginTV, Api_RemoveWebcast);
}

PLUGIN_BUNNY_API_CALL(PluginTV::Api_AddWebcast)
{
	Q_UNUSED(account);

	if(!hRequest.HasArg("time"))
		return new ApiManager::ApiError(QString("Missing argument 'time' for plugin TV"));

	if(!bunny->IsConnected())
		return new ApiManager::ApiError(QString("Bunny '%1' is not connected").arg(hRequest.GetArg("to")));

	QString hTime = hRequest.GetArg("time");
	if(!bunny->GetPluginSetting(GetName(), "Webcast/List", QStringList()).toStringList().contains(hTime))
	{
		QStringList time = hTime.split(":");
		int id = Cron::RegisterDaily(this, QTime(time[0].toInt(), time[1].toInt()), QVariant::fromValue(bunny));
		webcastList.insert(bunny , qMakePair(id, hTime));
		QStringList bunnyWebcastList = bunny->GetPluginSetting(GetName(), "Webcast/List", QStringList()).toStringList() << hTime;
		bunnyWebcastList.sort();
		bunny->SetPluginSetting(GetName(), "Webcast/List", bunnyWebcastList);
	}
	return new ApiManager::ApiString(QString("Add webcast at '%1' to bunny '%2'").arg(hRequest.GetArg("time"), QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginTV::Api_RemoveWebcast)
{
	Q_UNUSED(account);

	if(!hRequest.HasArg("time"))
		return new ApiManager::ApiError(QString("Missing argument 'time' for plugin TV"));

	if(!bunny->IsConnected())
		return new ApiManager::ApiError(QString("Bunny '%1' is not connected").arg(QString(bunny->GetID())));

	int remove = 0;
	QMultiMap<Bunny*, QPair<int, QString> >::iterator i = webcastList.find(bunny);
	while (i != webcastList.end() && i.key() == bunny)
	{
		if(i.value().second == hRequest.GetArg("time"))
		{
			Cron::Unregister(this, i->first);
			i = webcastList.erase(i);
			remove++;
		}
		else
			++i;
	}

	QStringList bunnyWebcastList = bunny->GetPluginSetting(GetName(), "Webcast/List", QStringList()).toStringList();
	bunnyWebcastList.removeAll(hRequest.GetArg("time"));
	bunnyWebcastList.sort();
	bunny->SetPluginSetting(GetName(), "Webcast/List", bunnyWebcastList);
	if(remove > 0)
		return new ApiManager::ApiString(QString("Remove webcast at '%1' for bunny '%2'").arg(hRequest.GetArg("time"), QString(bunny->GetID())));
	return new ApiManager::ApiError(QString("No webcast at '%1' for bunny '%2'").arg(hRequest.GetArg("time"), QString(bunny->GetID())));
}

/* WORKER THREAD */
PluginTV_Worker::PluginTV_Worker(PluginTV * p, Bunny * bu, QByteArray b):plugin(p),bunny(bu),buffer(b.replace("&amp;", "and"))
{
}

void PluginTV_Worker::run()
{
	QXmlStreamReader xml;
	xml.clear();
	xml.addData(buffer);

	QString currentTag;
	QString chaine;
	QByteArray message = "MU "+plugin->GetBroadcastHTTPPath("cesoir.mp3")+"\nPL 3\nMW\n";
	while (!xml.atEnd())
	{
		xml.readNext();
		if (xml.isStartElement())
		{
			currentTag = xml.name().toString();
		}
		else if (xml.isCharacters() && !xml.isWhitespace())
		{
			if (currentTag == "title")
			{
				QString title = xml.text().toString();
				QRegExp rx("(\\d\\d):(\\d\\d) : (.*) \\((.*)\\)");
				if(rx.indexIn(title) != -1)
				{
					if(chaine != rx.cap(4))
					{
						chaine = rx.cap(4);
						QString chaineFile = chaine;
						chaineFile = chaineFile.replace(" ", "").trimmed().append(".mp3").toLower();
						// Log::Debug(rx.cap(4) +" : "+rx.cap(3));
						QByteArray fileName = QCryptographicHash::hash(rx.cap(3).trimmed().toAscii(), QCryptographicHash::Md5).toHex().append(".mp3");
						TTSManager::CreateNewSound(rx.cap(4).trimmed(), "claire", plugin->tvFolder.absoluteFilePath(chaineFile));
						TTSManager::CreateNewSound(rx.cap(3).trimmed(), "julie", plugin->tvFolder.absoluteFilePath(fileName));
						message += "MU "+plugin->GetBroadcastHTTPPath(chaineFile)+"\nPL 3\nMW\nMU "+plugin->GetBroadcastHTTPPath(fileName)+"\nMW\n";
					}
				}
			}
		}
	}

	if (xml.error() && xml.error() != QXmlStreamReader::PrematureEndOfDocumentError)
		emit done(false, bunny, QByteArray());
	else
		emit done(true, bunny, message);
}
