#include <QDateTime>
#include <QCryptographicHash>
#include <QXmlStreamReader>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMapIterator>
#include <QRegExp>
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

PluginWeather::PluginWeather():PluginInterface("weather", "Weather", BunnyZtampPlugin)
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
	getWeatherPage(b, ville);
}


bool PluginWeather::OnRFID(Bunny * b, QByteArray const& tag)
{
	QString city = b->GetPluginSetting(GetName(), QString("RFIDWeather/%1").arg(QString(tag.toHex())), QString()).toString();
	if(city != "")
	{
		getWeatherPage(b, city);
		return true;
	}
	return false;
}

bool PluginWeather::OnClick(Bunny * b, PluginInterface::ClickType type)
{
	if (type == PluginInterface::SingleClick) {
		QString city = b->GetPluginSetting(GetName(), "Default/City", "").toString();
		if(city != "") {
			getWeatherPage(b, city);
			return true;
		}
	}
	return false;
}

void PluginWeather::getWeatherPage(Bunny * b, QString ville)
{
	Q_UNUSED(b);
	QUrl url("http://www.google.com/ig/api");
	url.addEncodedQueryItem("hl", b->GetPluginSetting(GetName(), "Lang","fr").toByteArray());
	url.addEncodedQueryItem("weather", QUrl::toPercentEncoding(ville));
	QNetworkAccessManager *manager = new QNetworkAccessManager(this);
	manager->setProperty("BunnyID", b->GetID());
	connect(manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(analyseXml(QNetworkReply*)));
	manager->get(QNetworkRequest(url));
}

void PluginWeather::analyseXml(QNetworkReply* networkReply)
{
	if (!networkReply->error()) {
		Bunny * bunny = BunnyManager::GetBunny(this, networkReply->parent()->property("BunnyID").toByteArray());
		if(bunny) {
			PluginWeather_Worker * p = new PluginWeather_Worker(this, bunny, QString::fromUtf8(networkReply->readAll()));
			connect(p, SIGNAL(done(bool,Bunny*,QByteArray)), this, SLOT(analyseDone(bool,Bunny*,QByteArray)));
			connect(p, SIGNAL(finished()), p, SLOT(deleteLater()));
			p->start();
		}
	}
	networkReply->deleteLater();
	networkReply->parent()->deleteLater();
}

void PluginWeather::analyseDone(bool ret, Bunny * b, QByteArray message)
{
	if(ret && b && b->IsIdle())
		 b->SendPacket(MessagePacket(message));
}

void PluginWeather::OnBunnyConnect(Bunny * b)
{
	QMap<QString, QVariant> list = b->GetPluginSetting(GetName(), "Webcasts", QMap<QString, QVariant>()).toMap();
	QMapIterator<QString, QVariant> i(list);
	while (i.hasNext()) {
		i.next();
		QString time = i.key();
		QString name = i.value().toString();
		Cron::RegisterDaily(this, QTime::fromString(time, "hh:mm"), b, QVariant::fromValue(name));
	}
}

void PluginWeather::OnBunnyDisconnect(Bunny * b)
{
	Cron::UnregisterAllForBunny(this, b);
}

void PluginWeather::InitApiCalls()
{
	DECLARE_PLUGIN_BUNNY_API_CALL("addrfid(tag,city)", PluginWeather, Api_AddRFID);
	DECLARE_PLUGIN_BUNNY_API_CALL("removerfid(tag)", PluginWeather, Api_RemoveRFID);
	DECLARE_PLUGIN_BUNNY_API_CALL("addcity(city)", PluginWeather, Api_addCity);
	DECLARE_PLUGIN_BUNNY_API_CALL("removecity(city)", PluginWeather, Api_removeCity);
	DECLARE_PLUGIN_BUNNY_API_CALL("getcitieslist()", PluginWeather, Api_getCitiesList);
	DECLARE_PLUGIN_BUNNY_API_CALL("setdefaultcity(city)", PluginWeather, Api_setDefaultCity);
	DECLARE_PLUGIN_BUNNY_API_CALL("getdefaultcity()", PluginWeather, Api_getDefaultCity);
	DECLARE_PLUGIN_BUNNY_API_CALL("addwebcast(time,city)", PluginWeather, Api_AddWebcast);
	DECLARE_PLUGIN_BUNNY_API_CALL("removewebcast(time)", PluginWeather, Api_RemoveWebcast);
	DECLARE_PLUGIN_BUNNY_API_CALL("getwebcastslist()", PluginWeather, Api_ListWebcast);
	DECLARE_PLUGIN_BUNNY_API_CALL("setlang(lg)", PluginWeather, Api_setLang);
	DECLARE_PLUGIN_BUNNY_API_CALL("getlang()", PluginWeather, Api_getLang);
}

PLUGIN_BUNNY_API_CALL(PluginWeather::Api_addCity) {
	Q_UNUSED(account);

	if(!hRequest.HasArg("city"))
		return new ApiManager::ApiError(QString("Missing argument 'city' for plugin Weather"));
	QString city = hRequest.GetArg("city");
	QStringList list = bunny->GetPluginSetting(GetName(), "Cities", QStringList()).toStringList();
	list.append(city);
	bunny->SetPluginSetting(GetName(), "Cities", list);

	return new ApiManager::ApiOk(QString("Added city '%1' for bunny '%2'").arg(city, QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginWeather::Api_removeCity)
{
	Q_UNUSED(account);

	if(!hRequest.HasArg("city"))
		return new ApiManager::ApiError(QString("Missing argument 'city' for plugin Weather"));

	QString city = hRequest.GetArg("city");
	QStringList list = bunny->GetPluginSetting(GetName(), "Cities", QStringList()).toStringList();
	list.removeAll(city);
	bunny->SetPluginSetting(GetName(), "Cities", list);

	return new ApiManager::ApiOk(QString("Removed city '%1' for bunny '%2'").arg(city, QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginWeather::Api_getCitiesList) {
	Q_UNUSED(account);
	Q_UNUSED(hRequest);
	return new ApiManager::ApiList(bunny->GetPluginSetting(GetName(), "Cities", QStringList()).toStringList());
}

PLUGIN_BUNNY_API_CALL(PluginWeather::Api_setDefaultCity)
{
	Q_UNUSED(account);

	if(!hRequest.HasArg("city"))
		return new ApiManager::ApiError(QString("Missing argument 'city' for plugin Weather"));

	bunny->SetPluginSetting(GetName(), "Default/City", hRequest.GetArg("city"));
	return new ApiManager::ApiOk(QString("New default city defined '%1' for bunny '%2'").arg(hRequest.GetArg("city"), QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginWeather::Api_getDefaultCity)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);

	return new ApiManager::ApiString(bunny->GetPluginSetting(GetName(), "Default/City",QString()).toString());
}

PLUGIN_BUNNY_API_CALL(PluginWeather::Api_AddWebcast)
{
	Q_UNUSED(account);

	QString hTime = hRequest.GetArg("time");
	QString city = hRequest.GetArg("city");
	QMap<QString, QVariant> list = bunny->GetPluginSetting(GetName(), "Webcasts", QMap<QString, QVariant>()).toMap();
	if(!list.contains(hTime))
	{
		Cron::RegisterDaily(this, QTime::fromString(hTime, "hh:mm"), bunny, QVariant::fromValue(city));
		list.insert(hTime,city);
		bunny->SetPluginSetting(GetName(), "Webcasts", list);
		return new ApiManager::ApiOk(QString("Add webcast at '%1' to bunny '%2'").arg(hTime, QString(bunny->GetID())));
	}
	return new ApiManager::ApiError(QString("Webcast already exists at '%1' for bunny '%2'").arg(hTime, QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginWeather::Api_RemoveWebcast)
{
	Q_UNUSED(account);

	if(!hRequest.HasArg("time"))
		return new ApiManager::ApiError(QString("Missing argument 'time' for plugin Weather"));

	QMap<QString, QVariant> list = bunny->GetPluginSetting(GetName(), "Webcasts", QMap<QString, QVariant>()).toMap();
	QString time = hRequest.GetArg("time");
    if(list.contains(time))
    {
		list.remove(time);
        bunny->SetPluginSetting(GetName(), "Webcasts", list);

		// Recreate crons
        OnBunnyDisconnect(bunny);
        OnBunnyConnect(bunny);
        return new ApiManager::ApiOk(QString("Remove webcast at '%1' for bunny '%2'").arg(hRequest.GetArg("time"), QString(bunny->GetID())));
    }
    return new ApiManager::ApiError(QString("No webcast at '%1' for bunny '%2'").arg(hRequest.GetArg("time"), QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginWeather::Api_ListWebcast)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);
	return new ApiManager::ApiMappedList(bunny->GetPluginSetting(GetName(), "Webcasts", QMap<QString, QVariant>()).toMap());
}

PLUGIN_BUNNY_API_CALL(PluginWeather::Api_AddRFID)
{
	Q_UNUSED(account);

	bunny->SetPluginSetting(GetName(), QString("RFIDWeather/%1").arg(hRequest.GetArg("tag")), hRequest.GetArg("city"));

	return new ApiManager::ApiOk(QString("Add weather for '%1' for RFID '%2', bunny '%3'").arg(hRequest.GetArg("city"), hRequest.GetArg("tag"), QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginWeather::Api_RemoveRFID)
{
	Q_UNUSED(account);

	bunny->RemovePluginSetting(GetName(), QString("RFIDWeather/%1").arg(hRequest.GetArg("tag")));

	return new ApiManager::ApiOk(QString("Remove RFID '%2' for bunny '%3'").arg(hRequest.GetArg("tag"), QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginWeather::Api_getLang)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);

	return new ApiManager::ApiString(bunny->GetPluginSetting(GetName(), "Lang","fr").toString());
}

PLUGIN_BUNNY_API_CALL(PluginWeather::Api_setLang)
{
	Q_UNUSED(account);
	bunny->SetPluginSetting(GetName(), "Lang",hRequest.GetArg("lg"));

	return new ApiManager::ApiOk("Lang Updated!");
}


/* WORKER THREAD */
PluginWeather_Worker::PluginWeather_Worker(PluginWeather * p, Bunny * bu, QString b):plugin(p),bunny(bu),buffer(b.replace("&amp;", "and").replace("</script>",""))
{

}

void PluginWeather_Worker::run()
{
	QXmlStreamReader xml;
	xml.clear();
	xml.addData(buffer);
	/*LogDebug("======================================");
	LogDebug(QString(buffer));
	LogDebug("======================================");*/
	QString currentTag;
	QString chaine;
	QByteArray message;
	char current = 0;
	bool doCurrent = false;
	char forecast = 0;
	bool doForecast = false;

	int iCurrentTemp=0;
	int iForecastLow=0;
	int iForecastHigh=0;
	QString sCurrentCond;
	QString sForecastCond;
	QString sForecast("Prévisions:");
	QString sCity;
	while (!xml.atEnd() && (current!=2 || forecast!=3))
	{
		xml.readNext();
		if (xml.isStartElement())
		{
			currentTag = xml.name().toString();
			if(currentTag == "postal_code")
				sCity =  xml.attributes().value("data").toString();

			if(current==0 && currentTag == "current_conditions") {
				doCurrent = true;
			}
			if(doCurrent) {
				if(currentTag == "condition") {
					sCurrentCond = xml.attributes().value("data").toString();
					current++;
				}else if(currentTag == "temp_c") {
					iCurrentTemp = xml.attributes().value("data").toString().toInt();
					current++;
				}
				if(current==2)
					doCurrent = false;
			}
			if(forecast==0 && currentTag == "forecast_conditions") {
				doForecast = true;
			}
			if(doForecast) {
				if(currentTag == "condition") {
					sForecastCond = xml.attributes().value("data").toString();
					forecast++;
				}else if(currentTag == "low") {
					iForecastLow = xml.attributes().value("data").toString().toInt();
					forecast++;
				}else if(currentTag == "high") {
					iForecastHigh = xml.attributes().value("data").toString().toInt();
					forecast++;
				}
				if(forecast==3)
					doForecast = false;
			}
		}
	}
	if (xml.error() && xml.error() != QXmlStreamReader::PrematureEndOfDocumentError)
	{
		LogDebug(QString("Error: %1").arg(xml.errorString()));
		emit done(false, bunny, QByteArray());
	}
	else
	{
		if(current==0 && forecast==0)
		{
			emit done(false, bunny, QByteArray());
		}
		else
		{
			QByteArray where = TTSManager::CreateNewSound(QString("Météo pour %1.").arg(sCity), "Claire");
			message += "MU "+where+"\nPL 3\nMW\n";
			if(current!=0)
			{
				QByteArray maintenant = TTSManager::CreateNewSound("actuellement, ", "Claire");
				QByteArray temperature = TTSManager::CreateNewSound(QString::number(iCurrentTemp) + " degrés", "Claire");
				QByteArray meteo = TTSManager::CreateNewSound(sCurrentCond, "Claire");
 				message += "MU "+maintenant+"\nPL 3\nMW\n";
 				message += "MU "+meteo+"\nPL 3\nMW\n";
 				message += "MU "+temperature+"\nPL 3\nMW\n";
			}
			if(forecast!=0)
			{
				QByteArray prevision = TTSManager::CreateNewSound(sForecast, "Claire");
				QByteArray meteo = TTSManager::CreateNewSound(sForecastCond, "Claire");
				QByteArray temperature = TTSManager::CreateNewSound("entre "+QString::number(iForecastLow) + " et "+QString::number(iForecastHigh) + " degrés", "Claire");
 				message += "MU "+prevision+"\nPL 3\nMW\n";
 				message += "MU "+meteo+"\nPL 3\nMW\n";
 				message += "MU "+temperature+"\nPL 3\nMW\n";
			}
			emit done(true, bunny, message);
		}
	}
}
