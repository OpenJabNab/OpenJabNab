#include <QDateTime>
#include <QCryptographicHash>
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
#include "plugin_airquality.h"
#include "settings.h"
#include "ttsmanager.h"


Q_EXPORT_PLUGIN2(plugin_airquality, PluginAirquality)

PluginAirquality::PluginAirquality():PluginInterface("airquality", "Air quality plugin", BunnyZtampPlugin)
{
	std::auto_ptr<QDir> dir(GetLocalHTTPFolder());
	if(dir.get())
	{
		airFolder = *dir;
	}
	cityList = GetSettings("config/city", QStringList()).toStringList();
	if(cityList.count() == 0)
	{
		QUrl url("http://www.buldair.org/scripts/datamap.php");
		QNetworkAccessManager *manager = new QNetworkAccessManager(this);
		connect(manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(analyseHtml(QNetworkReply*)));
		manager->setProperty("city", "");
		QNetworkRequest request;
		request.setRawHeader("Host", "www.buldair.org");
		request.setUrl(QUrl(url));
		manager->get(QNetworkRequest(url));
	}
}

PluginAirquality::~PluginAirquality()
{
	Cron::UnregisterAll(this);
}

void PluginAirquality::OnCron(Bunny * b, QVariant v)
{
	QString ville = v.value<QString>();
	getAirPage(b, ville);
}


bool PluginAirquality::OnRFID(Bunny * b, QByteArray const& tag)
{
	QString city = b->GetPluginSetting(GetName(), QString("RFIDAir/%1").arg(QString(tag.toHex())), QString()).toString();
	if(city != "")
	{
		getAirPage(b, city);
		return true;
	}
	return false;
}

bool PluginAirquality::OnClick(Bunny * b, PluginInterface::ClickType type)
{
	if (type == PluginInterface::SingleClick) {
		QString city = b->GetPluginSetting(GetName(), "Default/City", "").toString();
		if(city != "") {
			getAirPage(b, city);
			return true;
		}
	}
	return false;
}


void PluginAirquality::getAirPage(Bunny * b, QString ville)
{
	Q_UNUSED(b);
	QUrl url("http://www.buldair.org/scripts/datamap.php");
	QNetworkAccessManager *manager = new QNetworkAccessManager(this);
	manager->setProperty("BunnyID", b->GetID());
	manager->setProperty("city", ville);
	connect(manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(analyseHtml(QNetworkReply*)));
	manager->get(QNetworkRequest(url));
}

void PluginAirquality::analyseHtml(QNetworkReply* networkReply)
{
	if (!networkReply->error()) {
		Bunny * bunny = BunnyManager::GetBunny(this, networkReply->parent()->property("BunnyID").toByteArray());
		QString city = networkReply->parent()->property("city").toString();
		if(bunny) {
			PluginAir_Worker * p = new PluginAir_Worker(this, bunny, city, networkReply->readAll());
			connect(p, SIGNAL(done(bool,Bunny*,QByteArray)), this, SLOT(analyseDone(bool,Bunny*,QByteArray)));
			connect(p, SIGNAL(finished()), p, SLOT(deleteLater()));
			p->start();
		}
	}
	networkReply->deleteLater();
	networkReply->parent()->deleteLater();
}

void PluginAirquality::analyseDone(bool ret, Bunny * b, QByteArray message)
{
	if(ret && b && b->IsIdle())
		 b->SendPacket(MessagePacket(message));
}

void PluginAirquality::OnBunnyConnect(Bunny * b)
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

void PluginAirquality::OnBunnyDisconnect(Bunny * b)
{
	Cron::UnregisterAllForBunny(this, b);
}


void PluginAirquality::InitApiCalls()
{
	DECLARE_PLUGIN_BUNNY_API_CALL("addrfid(tag,city)", PluginAirquality, Api_AddRFID);
	DECLARE_PLUGIN_BUNNY_API_CALL("removerfid(tag)", PluginAirquality, Api_RemoveRFID);
	DECLARE_PLUGIN_BUNNY_API_CALL("addcity(city)", PluginAirquality, Api_addCity);
	DECLARE_PLUGIN_BUNNY_API_CALL("removecity(city)", PluginAirquality, Api_removeCity);
	DECLARE_PLUGIN_BUNNY_API_CALL("getcitieslist()", PluginAirquality, Api_getCitiesList);
	DECLARE_PLUGIN_BUNNY_API_CALL("setdefaultcity(city)", PluginAirquality, Api_setDefaultCity);
	DECLARE_PLUGIN_BUNNY_API_CALL("getdefaultcity()", PluginAirquality, Api_getDefaultCity);
	DECLARE_PLUGIN_BUNNY_API_CALL("addwebcast(time,city)", PluginAirquality, Api_AddWebcast);
	DECLARE_PLUGIN_BUNNY_API_CALL("removewebcast(time)", PluginAirquality, Api_RemoveWebcast);
	DECLARE_PLUGIN_BUNNY_API_CALL("getwebcastslist()", PluginAirquality, Api_ListWebcast);
}

PLUGIN_BUNNY_API_CALL(PluginAirquality::Api_addCity) {
	Q_UNUSED(account);

	if(!hRequest.HasArg("city"))
		return new ApiManager::ApiError(QString("Missing argument 'city' for plugin Airquality"));
	QString city = hRequest.GetArg("city");
	QStringList list = bunny->GetPluginSetting(GetName(), "Cities", QStringList()).toStringList();
	list.append(city);
	bunny->SetPluginSetting(GetName(), "Cities", list);

	return new ApiManager::ApiOk(QString("Added city '%1' for bunny '%2'").arg(city, QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginAirquality::Api_removeCity)
{
	Q_UNUSED(account);

	if(!hRequest.HasArg("city"))
		return new ApiManager::ApiError(QString("Missing argument 'city' for plugin Airquality"));

	QString city = hRequest.GetArg("city");
	QStringList list = bunny->GetPluginSetting(GetName(), "Cities", QStringList()).toStringList();
	list.removeAll(city);
	bunny->SetPluginSetting(GetName(), "Cities", list);

	return new ApiManager::ApiOk(QString("Removed city '%1' for bunny '%2'").arg(city, QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginAirquality::Api_getCitiesList) {
	Q_UNUSED(account);
	Q_UNUSED(hRequest);
	Q_UNUSED(bunny);
	return new ApiManager::ApiList(cityList);
}

PLUGIN_BUNNY_API_CALL(PluginAirquality::Api_setDefaultCity)
{
	Q_UNUSED(account);

	if(!hRequest.HasArg("city"))
		return new ApiManager::ApiError(QString("Missing argument 'city' for plugin Airquality"));

	bunny->SetPluginSetting(GetName(), "Default/City", hRequest.GetArg("city"));
	return new ApiManager::ApiOk(QString("New default city defined '%1' for bunny '%2'").arg(hRequest.GetArg("city"), QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginAirquality::Api_getDefaultCity)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);

	return new ApiManager::ApiString(bunny->GetPluginSetting(GetName(), "Default/City",QString()).toString());
}

PLUGIN_BUNNY_API_CALL(PluginAirquality::Api_AddWebcast)
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

PLUGIN_BUNNY_API_CALL(PluginAirquality::Api_RemoveWebcast)
{
	Q_UNUSED(account);

	if(!hRequest.HasArg("time"))
		return new ApiManager::ApiError(QString("Missing argument 'time' for plugin Airquality"));

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

PLUGIN_BUNNY_API_CALL(PluginAirquality::Api_ListWebcast)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);
	return new ApiManager::ApiMappedList(bunny->GetPluginSetting(GetName(), "Webcasts", QMap<QString, QVariant>()).toMap());
}

PLUGIN_BUNNY_API_CALL(PluginAirquality::Api_AddRFID)
{
	Q_UNUSED(account);

	bunny->SetPluginSetting(GetName(), QString("RFIDAirquality/%1").arg(hRequest.GetArg("tag")), hRequest.GetArg("city"));

	return new ApiManager::ApiOk(QString("Add weather for '%1' for RFID '%2', bunny '%3'").arg(hRequest.GetArg("city"), hRequest.GetArg("tag"), QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginAirquality::Api_RemoveRFID)
{
	Q_UNUSED(account);

	bunny->RemovePluginSetting(GetName(), QString("RFIDAirquality/%1").arg(hRequest.GetArg("tag")));

	return new ApiManager::ApiOk(QString("Remove RFID '%2' for bunny '%3'").arg(hRequest.GetArg("tag"), QString(bunny->GetID())));
}



/* WORKER THREAD */
PluginAir_Worker::PluginAir_Worker(PluginAirquality * p, Bunny * bu, QString c, QString b):plugin(p),bunny(bu),city(c),buffer(b)
{

}

void PluginAir_Worker::run()
{
	qualities << "inconnue" << "très bonne" << "très bonne" << "bonne" << "bonne" << "moyenne" << "médiocre" << "médiocre" << "mauvaise" << "mauvaise" << "très mauvaise";
	QUrl* data = new QUrl("http://data/?" + buffer);
	QStringList cities = data->queryItemValue("NomVille").toLower().split(",");
	plugin->SetSettings("config/city", cities);

	if(city != "")
	{
		int index = cities.indexOf(city);
		int quality;
		QByteArray message;
		if(index != -1)
		{
			QStringList i1 = data->queryItemValue("I1").split(",", QString::KeepEmptyParts);
			QStringList i2 = data->queryItemValue("I2").split(",", QString::KeepEmptyParts);
			quality = (i2.at(index) != "" ? i2.at(index) : i1.at(index)).toInt();
		}
		else
		{
			quality = 0;
		}
		QString text = QString::fromUtf8(("La qualité de l'air à "+city+" est "+qualities.at(quality)).toAscii());
		QByteArray q = TTSManager::CreateNewSound(text, "Claire");
		message = "MU "+q+"\nPL 3\nMW\n";
		emit done(true, bunny, message);
	}
	else
	{
		emit done(false, bunny, QByteArray());
	}
}
