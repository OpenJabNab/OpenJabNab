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
#include "plugin_weather.h"
#include "settings.h"
#include "ttsmanager.h"

Q_EXPORT_PLUGIN2(plugin_weather, PluginWeather)

PluginWeather::PluginWeather():PluginInterface("weather", "Météo",BunnyZtampPlugin)
{
	std::auto_ptr<QDir> dir(GetLocalHTTPFolder());
	if(dir.get())
	{
		weatherFolder = *dir;
	}
}

PluginWeather::~PluginWeather()
{
	Cron::UnregisterAll(this);
}

void PluginWeather::OnCron(Bunny * b, QVariant v)
{
	QString ville = v.value<QString>();
	//Bunny * b = QVariantHelper::ToBunnyPtr(v);
	LogDebug(ville);
	getWeatherPage(b, ville);
}

bool PluginWeather::OnClick(Bunny * b, PluginInterface::ClickType type)
{
	if (type == PluginInterface::SingleClick)
	{
		getWeatherPage(b, "612977");
		return true;
	}
	return false;
}

void PluginWeather::getWeatherPage(Bunny * b, QString ville)
{
	QHttp* http = new QHttp(this);
	http->setProperty("BunnyID", b->GetID());
	connect(http, SIGNAL(done(bool)), this, SLOT(analyseXml()));
	http->setHost("weather.yahooapis.com");
	http->get("/forecastrss?w="+ville+"&u=c");
}

void PluginWeather::analyseXml()
{
	std::auto_ptr<QHttp> http(qobject_cast<QHttp *>(sender()));
	Bunny * bunny = BunnyManager::GetBunny(this, http->property("BunnyID").toByteArray());
	if(!bunny)
		return;

	PluginWeather_Worker * p = new PluginWeather_Worker(this, bunny, http->readAll());
	connect(p, SIGNAL(done(bool,Bunny*,QByteArray)), this, SLOT(analyseDone(bool,Bunny*,QByteArray)));
	connect(p, SIGNAL(finished()), p, SLOT(deleteLater()));
	p->start();
}

void PluginWeather::analyseDone(bool ret, Bunny * b, QByteArray message)
{
	if(ret && b->IsConnected())
		 b->SendPacket(MessagePacket(message));
}

void PluginWeather::OnBunnyConnect(Bunny * b)
{
	if(!webcastList.contains(b))
	{
		QStringList webcasts = b->GetPluginSetting(GetName(), "Webcast/List", QStringList()).toStringList();
		foreach(QString webcast, webcasts)
		{
			QStringList time = webcast.split(":");
			int id = Cron::Register(this, 60*24, time[0].toInt(), time[1].toInt(), b, QVariant::fromValue( QString("612977") ));
			webcastList.insert(b , qMakePair(id, webcast));
		}
	}
}

void PluginWeather::OnBunnyDisconnect(Bunny * b)
{
	typedef QPair<int, QString> listElement;
	QList<listElement> listOfWebcasts = webcastList.values(b);
	foreach(listElement l, listOfWebcasts)
	{
		Cron::Unregister(this, l.first);
	}
	webcastList.remove(b);
}

void PluginWeather::InitApiCalls()
{
	DECLARE_PLUGIN_BUNNY_API_CALL("defaultcity(city)", PluginWeather, Api_DefaultCity);
	DECLARE_PLUGIN_BUNNY_API_CALL("addwebcast(time,city)", PluginWeather, Api_AddWebcast);
	DECLARE_PLUGIN_BUNNY_API_CALL("removewebcast(time,city)", PluginWeather, Api_RemoveWebcast);
	DECLARE_PLUGIN_BUNNY_API_CALL("listwebcast()", PluginWeather, Api_ListWebcast);
}

PLUGIN_BUNNY_API_CALL(PluginWeather::Api_DefaultCity)
{
	Q_UNUSED(account);

	if(!hRequest.HasArg("city"))
		return new ApiManager::ApiError(QString("Missing argument 'city' for plugin Weather"));

	if(!bunny->IsConnected())
		return new ApiManager::ApiError(QString("Bunny '%1' is not connected").arg(hRequest.GetArg("to")));

	bunny->SetPluginSetting(GetName(), "Default/City", hRequest.GetArg("city"));
	return new ApiManager::ApiOk(QString("New default city defined '%1' for bunny '%2'").arg(hRequest.GetArg("city"), QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginWeather::Api_AddWebcast)
{
	Q_UNUSED(account);
/*
	if(!hRequest.HasArg("time"))
		return new ApiManager::ApiError(QString("Missing argument 'time' for plugin Weather"));

	if(!hRequest.HasArg("city"))
		return new ApiManager::ApiError(QString("Missing argument 'city' for plugin Weather"));
*/
	if(!bunny->IsConnected())
		return new ApiManager::ApiError(QString("Bunny '%1' is not connected").arg(hRequest.GetArg("to")));

	QString hTime = hRequest.GetArg("time");
	if(!bunny->GetPluginSetting(GetName(), "Webcast/List", QStringList()).toStringList().contains(hTime))
	{
		QStringList time = hTime.split(":");
		int id = Cron::RegisterDaily(this, QTime(time[0].toInt(), time[1].toInt()), bunny, QVariant::fromValue( QString("612977") ));
		webcastList.insert(bunny , qMakePair(id, hTime));
		QStringList bunnyWebcastList = bunny->GetPluginSetting(GetName(), "Webcast/List", QStringList()).toStringList() << hTime;
		bunnyWebcastList.sort();
		bunny->SetPluginSetting(GetName(), "Webcast/List", bunnyWebcastList);
	}
	return new ApiManager::ApiOk(QString("Add webcast at '%1' to bunny '%2'").arg(hRequest.GetArg("time"), QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginWeather::Api_RemoveWebcast)
{
	Q_UNUSED(account);

	if(!hRequest.HasArg("time"))
		return new ApiManager::ApiError(QString("Missing argument 'time' for plugin Weather"));

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
		return new ApiManager::ApiOk(QString("Remove webcast at '%1' for bunny '%2'").arg(hRequest.GetArg("time"), QString(bunny->GetID())));
	return new ApiManager::ApiError(QString("No webcast at '%1' for bunny '%2'").arg(hRequest.GetArg("time"), QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginWeather::Api_ListWebcast)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);

	if(!bunny->IsConnected())
		return new ApiManager::ApiError(QString("Bunny '%1' is not connected").arg(QString(bunny->GetID())));

	//QMap<QString, QString> list;
	QList<QString> list;
	foreach (QString webcast, bunny->GetPluginSetting(GetName(), "Webcast/List", QStringList()).toStringList())
		list.append(webcast);
		//list.insert(webcast, ville);

	//return new ApiManager::ApiMappedList(list);
	return new ApiManager::ApiList(list);
}

/* WORKER THREAD */
PluginWeather_Worker::PluginWeather_Worker(PluginWeather * p, Bunny * bu, QByteArray b):plugin(p),bunny(bu),buffer(b.replace("&amp;", "and"))
{
	weatherCodes << "tornade" << "tornade" << "ouragans" << "orages" << "orages";
	weatherCodes << "neige et pluie" << "neige et pluie" << "neige et pluie" << "bruine" << "bruine";
	weatherCodes << "pluie verglaçante" << "averses" << "averses" << "averses de neige" << "averses de neige";
	weatherCodes << "neige" << "neige" << "grêle" << "neige" << "brouillard";
	weatherCodes << "brouillard" << "brume" << "brume" << "vent" << "vent";
	weatherCodes << "froid" << "nuageux" << "nuageux" << "nuageux" << "nuageux";
	weatherCodes << "nuageux" << "dégagé" << "ensoleillé" << "ensoleillé" << "ensoleillé";
	weatherCodes << "pluie et grêle" << "chaud" << "orages isolés" << "orages isolés" << "orages isolés";
	weatherCodes << "averses" << "neige" << "averses de neige" << "neige" << "nuageux";
	weatherCodes << "orages" << "averses de neige" << "orages" << "pas de prévisions";
}

void PluginWeather_Worker::run()
{
	QXmlStreamReader xml;
	xml.clear();
	xml.addData(buffer);

	QString currentTag;
	QString chaine;
	QByteArray message;
	int iForecast = 0;
	bool current = false;
	bool forecast = false;

	int iCurrentCode=0;
	int iCurrentTemp=0;
	int iForecastCode=0;
	int iForecastMin=0;
	int iForecastMax=0;
	QString sForecast;
	while (!xml.atEnd())
	{
		xml.readNext();
		if (xml.isStartElement())
		{
			currentTag = xml.name().toString();
			if(currentTag == "condition")
			{
				current = true;
				QXmlStreamAttributes attrs = xml.attributes();
				iCurrentCode = attrs.value("code").toString().toInt();
				iCurrentTemp = attrs.value("temp").toString().toInt();

			}
			if(currentTag == "forecast")
			{
				iForecast++;
				// before 15:00, gives current and day forecast
				if(QDateTime::currentDateTime().toString("hh").toInt() <= 14 && iForecast == 1)
				{
					forecast = true;
					sForecast = "Prévisions pour aujourd'hui";
					QXmlStreamAttributes attrs = xml.attributes();
					iForecastCode = attrs.value("code").toString().toInt();
					iForecastMin = attrs.value("low").toString().toInt();
					iForecastMax = attrs.value("high").toString().toInt();
				}
				else if(QDateTime::currentDateTime().toString("hh").toInt() > 14 && iForecast == 2)
				{
					forecast = true;
					sForecast = "Prévisions pour demain";
					QXmlStreamAttributes attrs = xml.attributes();
					iForecastCode = attrs.value("code").toString().toInt();
					iForecastMin = attrs.value("low").toString().toInt();
					iForecastMax = attrs.value("high").toString().toInt();
				}

			}
		}
	}

	if (xml.error() && xml.error() != QXmlStreamReader::PrematureEndOfDocumentError)
	{
		emit done(false, bunny, QByteArray());
	}
	else
	{
		if(current || forecast)
		{
			if(current)
			{

				QByteArray maintenant = TTSManager::CreateNewSound("actuellement, ", "claire");
				QByteArray meteo = TTSManager::CreateNewSound(QString::number(iCurrentTemp) + " degrés", "claire");
				QByteArray temperature = TTSManager::CreateNewSound(weatherCodes.at(iCurrentCode), "claire");
 				message += "MU "+maintenant+"\nPL 3\nMW\n";
 				message += "MU "+meteo+"\nPL 3\nMW\n";
 				message += "MU "+temperature+"\nPL 3\nMW\n";
			}
			if(forecast)
			{
				QByteArray prevision = TTSManager::CreateNewSound(sForecast, "claire");
				QByteArray meteo = TTSManager::CreateNewSound(weatherCodes.at(iForecastCode), "claire");
				QByteArray temperature = TTSManager::CreateNewSound("entre "+QString::number(iForecastMin) + " et "+QString::number(iForecastMax) + " degrés", "claire");
 				message += "MU "+prevision+"\nPL 3\nMW\n";
 				message += "MU "+meteo+"\nPL 3\nMW\n";
 				message += "MU "+temperature+"\nPL 3\nMW\n";
			}
			emit done(true, bunny, message);
		}
		else
		{
			emit done(false, bunny, QByteArray());
		}
	}
}
/*
Weather_Sun = 0, Weather_Cloudy, Weather_Smog, Weather_Rain, Weather_Snow, Weather_Storm
0 tornade
1 tornade tropicale
2 ouragans
3 orages violents
4 orages

5 pluie et neige
6 pluie et grésil
7 neige et grésil
8 bruine verglaçante
9 bruine

10 pluie verglaçante
11 douches
12 douches
13 averses de neige
14 légères averses de neige

15 poudrerie
16 neige
17 grêle
18 grésil
19 brouillard

20 brouillard
21 brume
22 brume
23 vent
24 vent

25 froid
26 nuageux
27 principalement nuageux
28 principalement nuageux
29 partiellement nuageux

30 partiellement nuageux
31 nuit claire
32 ensoleillé
33 ensoleillé
34 ensoleillé

35 pluie mêlée de grêle
36 chaud
37 orages isolés
38 orages dispersés
39 orages dispersés

40 averses dispersés
41 fortes chutes de neige
42 des averses de neige dispersées
43 fortes chutes de neige
44 nuageux

45 orages
46 averses de neige
47 orages isolés
3200 pas disponible
*/
