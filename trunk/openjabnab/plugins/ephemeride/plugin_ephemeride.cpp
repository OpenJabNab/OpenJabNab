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
#include "plugin_ephemeride.h"
#include "settings.h"
#include "ttsmanager.h"

Q_EXPORT_PLUGIN2(plugin_ephemeride, PluginEphemeride)

PluginEphemeride::PluginEphemeride():PluginInterface("ephemeride", "Ephemeride") {}

PluginEphemeride::~PluginEphemeride()
{
	Cron::UnregisterAll(this);
}

bool PluginEphemeride::Init()
{
	std::auto_ptr<QDir> dir(GetLocalHTTPFolder());
	if(dir.get())
	{
		ephemerideFolder = *dir;
		TTSManager::CreateNewSound("Aujourd'hui nous fetons les", "claire", ephemerideFolder.absoluteFilePath("aujourdhui.mp3"));
		return true;
	}
	return false;
}

void PluginEphemeride::OnCron(QVariant v)
{
	Bunny * b = QVariantHelper::ToBunnyPtr(v);
	getEphemeridePage(b);
}

bool PluginEphemeride::OnClick(Bunny * b, PluginInterface::ClickType type)
{
	if (type == PluginInterface::SingleClick && b->GetGlobalSetting("singleClickPlugin", "").toByteArray() == GetName())
	{
		getEphemeridePage(b);
		return true;
	}
	return false;
}

void PluginEphemeride::getEphemeridePage(Bunny * b)
{
	QHttp* http = new QHttp(this);
	http->setProperty("BunnyID", b->GetID());
	connect(http, SIGNAL(done(bool)), this, SLOT(analyseXml()));
	//http://www.net-pratique.fr/services/saintdujour.php
	http->setHost("www.net-pratique.fr");
	http->get("/services/saintdujour.php");
}

void PluginEphemeride::analyseXml()
{
	std::auto_ptr<QHttp> http(qobject_cast<QHttp *>(sender()));
	Bunny * bunny = BunnyManager::GetBunny(this, http->property("BunnyID").toByteArray());
	if(!bunny)
		return;

	PluginEphemeride_Worker * p = new PluginEphemeride_Worker(this, bunny, http->readAll());
	connect(p, SIGNAL(done(bool,Bunny*,QByteArray)), this, SLOT(analyseDone(bool,Bunny*,QByteArray)));
	connect(p, SIGNAL(finished()), p, SLOT(deleteLater()));
	p->start();
}

void PluginEphemeride::analyseDone(bool ret, Bunny * b, QByteArray message)
{
	if(ret && b->IsConnected())
		 b->SendPacket(MessagePacket(message));
}

void PluginEphemeride::OnBunnyConnect(Bunny * b)
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

void PluginEphemeride::OnBunnyDisconnect(Bunny * b)
{
	typedef QPair<int, QString> listElement;
	QList<listElement> listOfWebcasts = webcastList.values(b);
	foreach(listElement l, listOfWebcasts)
	{
		Cron::Unregister(this, l.first);
	}
	webcastList.remove(b);
}

void PluginEphemeride::InitApiCalls()
{
	DECLARE_PLUGIN_BUNNY_API_CALL("addwebcast", PluginEphemeride, Api_AddWebcast);
	DECLARE_PLUGIN_BUNNY_API_CALL("removewebcast", PluginEphemeride, Api_RemoveWebcast);
}

PLUGIN_BUNNY_API_CALL(PluginEphemeride::Api_AddWebcast)
{
	Q_UNUSED(account);

	if(!hRequest.HasArg("time"))
		return new ApiManager::ApiError(QString("Missing argument 'time' for plugin Ephemeride"));

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

PLUGIN_BUNNY_API_CALL(PluginEphemeride::Api_RemoveWebcast)
{
	Q_UNUSED(account);

	if(!hRequest.HasArg("time"))
		return new ApiManager::ApiError(QString("Missing argument 'time' for plugin Ephemeride"));

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
PluginEphemeride_Worker::PluginEphemeride_Worker(PluginEphemeride * p, Bunny * bu, QByteArray b):plugin(p),bunny(bu),buffer(b.replace("&amp;", "and"))
{
}

void PluginEphemeride_Worker::run()
{
	QByteArray message = "MU "+plugin->GetBroadcastHTTPPath("aujourdhui.mp3")+"\nPL 3\nMW\n";
	QRegExp rx(">(.*)</span>");
	if(rx.indexIn(buffer) != -1)
	{
		QString prenomFile = rx.cap(1);
		prenomFile = prenomFile.replace(" ", "").replace("-", "").trimmed().append(".mp3").toLower();
		TTSManager::CreateNewSound(rx.cap(1).trimmed(), "claire", plugin->ephemerideFolder.absoluteFilePath(prenomFile));
		message += "MU "+plugin->GetBroadcastHTTPPath(prenomFile)+"\nPL 3\nMW\n";
		emit done(true, bunny, message);
	}
	else
		emit done(false, bunny, QByteArray());
}
