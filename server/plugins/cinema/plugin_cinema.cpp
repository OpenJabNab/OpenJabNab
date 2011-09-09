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
#include "plugin_cinema.h"
#include "settings.h"
#include "ttsmanager.h"

Q_EXPORT_PLUGIN2(plugin_cinema, PluginCinema)

PluginCinema::PluginCinema():PluginInterface("cinema", "Sorties cinema de la semaine",BunnyZtampPlugin) { }

PluginCinema::~PluginCinema()
{
	Cron::UnregisterAll(this);
}

void PluginCinema::OnCron(Bunny * b, QVariant)
{
	getCinemaPage(b);
}

bool PluginCinema::OnClick(Bunny * b, PluginInterface::ClickType type)
{
	if (type == PluginInterface::SingleClick)
	{
		getCinemaPage(b);
		return true;
	}
	return false;
}

void PluginCinema::getCinemaPage(Bunny * b)
{
	QHttp* http = new QHttp(this);
	http->setProperty("BunnyID", b->GetID());
	connect(http, SIGNAL(done(bool)), this, SLOT(analyseXml()));
	http->setHost("www.cinefil.com");
	http->get("/rss-sorties-cinema-de-la-semaine");
}

void PluginCinema::analyseXml()
{
	std::auto_ptr<QHttp> http(qobject_cast<QHttp *>(sender()));
	Bunny * bunny = BunnyManager::GetBunny(this, http->property("BunnyID").toByteArray());
	if(!bunny)
		return;

	PluginCinema_Worker * p = new PluginCinema_Worker(this, bunny, http->readAll());
	connect(p, SIGNAL(done(bool,Bunny*,QByteArray)), this, SLOT(analyseDone(bool,Bunny*,QByteArray)));
	connect(p, SIGNAL(finished()), p, SLOT(deleteLater()));
	p->start();
}

void PluginCinema::analyseDone(bool ret, Bunny * b, QByteArray message)
{
	if(ret && b->IsConnected())
		 b->SendPacket(MessagePacket(message));
}

void PluginCinema::OnBunnyConnect(Bunny * b)
{
	QStringList webcasts = b->GetPluginSetting(GetName(), "Webcast/List", QStringList()).toStringList();
	foreach(QString webcast, webcasts)
	{
		QStringList wc = webcast.split("|");
		Cron::RegisterWeekly(this, Qt::DayOfWeek(wc[0].toInt()), QTime::fromString(wc[1], "hh:mm"), b);
	}
}

void PluginCinema::OnBunnyDisconnect(Bunny * b)
{
	Cron::UnregisterAllForBunny(this, b);
}

/*******
 * API *
 *******/

void PluginCinema::InitApiCalls()
{
	DECLARE_PLUGIN_BUNNY_API_CALL("addwebcast(day,time)", PluginCinema, Api_AddWebcast);
	DECLARE_PLUGIN_BUNNY_API_CALL("removewebcast(day,time)", PluginCinema, Api_RemoveWebcast);
	DECLARE_PLUGIN_BUNNY_API_CALL("getwebcastslist()", PluginCinema, Api_GetWebcasts);
}

PLUGIN_BUNNY_API_CALL(PluginCinema::Api_GetWebcasts)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);
	return new ApiManager::ApiList(bunny->GetPluginSetting(GetName(), "Webcast/List", QStringList()).toStringList());
}

PLUGIN_BUNNY_API_CALL(PluginCinema::Api_AddWebcast)
{
	Q_UNUSED(account);

	if(!bunny->IsConnected())
		return new ApiManager::ApiError(QString("Bunny '%1' is not connected").arg(hRequest.GetArg("to")));

	QString hTime = hRequest.GetArg("time");
	QString hDay = hRequest.GetArg("day");
	if(!bunny->GetPluginSetting(GetName(), "Webcast/List", QStringList()).toStringList().contains(hTime))
	{
		Cron::RegisterWeekly(this, Qt::DayOfWeek(hDay.toInt()), QTime::fromString(hTime, "hh:mm"), bunny);
		QStringList bunnyWebcastList = bunny->GetPluginSetting(GetName(), "Webcast/List", QStringList()).toStringList() << hDay+"|"+hTime;
		bunnyWebcastList.sort();
		bunny->SetPluginSetting(GetName(), "Webcast/List", bunnyWebcastList);
		return new ApiManager::ApiOk(QString("Add webcast at '%1' to bunny '%2'").arg(hRequest.GetArg("time"), QString(bunny->GetID())));
	}
	return new ApiManager::ApiError(QString("Webcast at '%1' already exists for bunny '%2'").arg(hRequest.GetArg("time"), QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginCinema::Api_RemoveWebcast)
{
	Q_UNUSED(account);

	if(!bunny->IsConnected())
		return new ApiManager::ApiError(QString("Bunny '%1' is not connected").arg(QString(bunny->GetID())));

	QStringList bunnyWebcastList = bunny->GetPluginSetting(GetName(), "Webcast/List", QStringList()).toStringList();
	QString time = hRequest.GetArg("day")+"|"+hRequest.GetArg("time");
	if(bunnyWebcastList.contains(time))
	{
		bunnyWebcastList.removeAll(time);
		bunnyWebcastList.sort();
		bunny->SetPluginSetting(GetName(), "Webcast/List", bunnyWebcastList);

		// Recreate crons
		OnBunnyDisconnect(bunny);
		OnBunnyConnect(bunny);

		return new ApiManager::ApiOk(QString("Remove webcast at '%1' on '%3' for bunny '%2'").arg(hRequest.GetArg("time"), QString(bunny->GetID()), hRequest.GetArg("day") ));
	}
	return new ApiManager::ApiError(QString("No webcast on '%2' at '%1' for bunny '%3'").arg(hRequest.GetArg("time"), hRequest.GetArg("day"), QString(bunny->GetID())));
}

/* WORKER THREAD */
PluginCinema_Worker::PluginCinema_Worker(PluginCinema * p, Bunny * bu, QByteArray b):plugin(p),bunny(bu),buffer(b.replace("&amp;", "and"))
{
}

void PluginCinema_Worker::run()
{
	QXmlStreamReader xml;
	xml.clear();
	xml.addData(buffer);

	QString currentTag;
	QString chaine = "";
	while (!xml.atEnd())
	{
		xml.readNext();
		if (xml.isStartElement())
		{
			currentTag = xml.name().toString();
		}
		else if (xml.isCharacters() && !xml.isWhitespace())
		{
			if (currentTag == "title" && xml.text().toString() != "cinefil.com")
			{
				QString title = xml.text().toString();
				chaine += title + ". .";
			}
		}
	}
	QByteArray file = TTSManager::CreateNewSound(chaine, "julie");
	QByteArray message = "MU " + file + "\nPL 3\nMW\n";

	if (xml.error() && xml.error() != QXmlStreamReader::PrematureEndOfDocumentError)
		emit done(false, bunny, QByteArray());
	else
		emit done(true, bunny, message);
}
