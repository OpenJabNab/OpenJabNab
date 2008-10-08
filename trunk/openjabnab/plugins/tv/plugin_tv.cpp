#include <QDateTime>
#include <QCryptographicHash>
#include <QXmlStreamReader>
#include <QHttp>
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
	QHttp * http = qobject_cast<QHttp *>(sender());
	Bunny * b = BunnyManager::GetBunny(http->property("BunnyID").toByteArray());
	QXmlStreamReader xml;
	xml.clear();
	QString source = http->readAll();
	xml.addData(source.replace("&amp;", "and"));
	delete http;

	QString currentTag;
	QString chaine;
	QByteArray message = "MU "+GetBroadcastHTTPPath("cesoir.mp3")+"\nPL 3\nMW\n";
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
						Log::Debug(rx.cap(4) +" : "+rx.cap(3));
						QByteArray fileName = QCryptographicHash::hash(rx.cap(3).trimmed().toAscii(), QCryptographicHash::Md5).toHex().append(".mp3");
						TTSManager::CreateNewSound(rx.cap(4).trimmed(), "claire", tvFolder.absoluteFilePath(chaineFile));
						TTSManager::CreateNewSound(rx.cap(3).trimmed(), "julie", tvFolder.absoluteFilePath(fileName));
						message += "MU "+GetBroadcastHTTPPath(chaineFile)+"\nPL 3\nMW\nMU "+GetBroadcastHTTPPath(fileName)+"\nMW\n";
					}
				}
			}
		}
	}

	if (xml.error() && xml.error() != QXmlStreamReader::PrematureEndOfDocumentError)
	{
		Log::Error(QString("Plugin " + GetVisualName() + " - XML Error at line %1 : %2").arg(xml.lineNumber()).arg(xml.errorString()));
	}
	else
	{
//		Log::Debug(message);
		b->SendPacket(MessagePacket(message));
	}
}

PluginTV::~PluginTV()
{
}

void PluginTV::OnBunnyConnect(Bunny * b)
{
	QStringList webcasts = b->GetPluginSetting(GetName(), "Webcast/List", QStringList()).toStringList();
	foreach(QString webcast, webcasts)
	{
		QStringList time = webcast.split(":");
		int id = Cron::Register(this, 60*24, time[0].toInt(), time[1].toInt(), QVariant::fromValue( b ));
		webcastList.insert(id, QStringList() << QString(b->GetID()) << webcast);
	}
}

ApiManager::ApiAnswer * PluginTV::ProcessApiCall(QByteArray const& funcName, HTTPRequest const& r)
{
	if(funcName.toLower() == "addwebcast")
	{
		if(!r.HasArg("to"))
			return new ApiManager::ApiError(QString("Missing argument 'to' for plugin TV"));
		if(!r.HasArg("time"))
			return new ApiManager::ApiError(QString("Missing argument 'time' for plugin TV"));
	
		Bunny * b = BunnyManager::GetConnectedBunny(r.GetArg("to").toAscii());
		if(!b)
			return new ApiManager::ApiError(QString("Bunny '%1' is not connected").arg(r.GetArg("to")));

		if(! b->GetPluginSetting(GetName(), "Webcast/List", QStringList()).toStringList().contains(r.GetArg("time")))
		{
			QStringList time = r.GetArg("time").split(":");
			int id = Cron::Register(this, 60*24, time[0].toInt(), time[1].toInt(), QVariant::fromValue( b ));
			webcastList.insert(id, QStringList() << r.GetArg("to") << r.GetArg("time"));
			QStringList bunnyWebcastList = b->GetPluginSetting(GetName(), "Webcast/List", QStringList()).toStringList() << r.GetArg("time");
			bunnyWebcastList.sort();
			b->SetPluginSetting(GetName(), "Webcast/List", bunnyWebcastList);
		}
		return new ApiManager::ApiString(QString("Add webcast at '%1' to bunny '%2'").arg(r.GetArg("time"), r.GetArg("to")));
	}
	else if(funcName.toLower() == "removewebcast")
	{
		if(!r.HasArg("to"))
			return new ApiManager::ApiError(QString("Missing argument 'to' for plugin TV"));
		if(!r.HasArg("time"))
			return new ApiManager::ApiError(QString("Missing argument 'time' for plugin TV"));
	
		Bunny * b = BunnyManager::GetConnectedBunny(r.GetArg("to").toAscii());
		if(!b)
			return new ApiManager::ApiError(QString("Bunny '%1' is not connected").arg(r.GetArg("to")));
		
		QMapIterator<int, QStringList> i(webcastList);
		int remove = 0;
		while (i.hasNext()) {
			i.next();
			if(i.value()[0] == r.GetArg("to") && i.value()[1] == r.GetArg("time"))
			{
				Cron::Unregister(this, i.key());
				webcastList.remove(i.key());
				remove++;
			}
		}
		QStringList bunnyWebcastList = b->GetPluginSetting(GetName(), "Webcast/List", QStringList()).toStringList();
		bunnyWebcastList.removeAll(r.GetArg("time"));
		bunnyWebcastList.sort();
		b->SetPluginSetting(GetName(), "Webcast/List", bunnyWebcastList);
		if(remove > 0)
			return new ApiManager::ApiString(QString("Remove webcast at '%1' for bunny '%2'").arg(r.GetArg("time"), r.GetArg("to")));
		return new ApiManager::ApiError(QString("No webcast at '%1' for bunny '%2'").arg(r.GetArg("time"), r.GetArg("to")));
	}
	else
		return new ApiManager::ApiError(QString("Bad function name for plugin TV"));
}
