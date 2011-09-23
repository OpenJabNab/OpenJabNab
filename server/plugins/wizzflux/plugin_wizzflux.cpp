#include <QMap>
#include <QMapIterator>
#include <QTime>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "account.h"
#include "plugin_wizzflux.h"
#include "bunny.h"
#include "cron.h"
#include "bunnymanager.h"
#include "messagepacket.h"

Q_EXPORT_PLUGIN2(plugin_wizzflux, PluginWizzflux)

PluginWizzflux::PluginWizzflux():PluginInterface("wizzflux", "Various Flux by Wizz.cc", BunnyZtampPlugin) {
	Flist = GetSettings("ListFlux", QStringList()).toStringList();
	if(Flist.count() == 0)
	{
		Flist << "lemonde_une" << "lemonde_economie" << "lemonde_culture" << "lemonde_planete";
		Flist << "lemonde_cinema" << "lemonde_livres" << "lemonde_sports" << "lemonde_voyages" << "lemonde_international";
		Flist << "gala" << "voici" << "yahoo_closer";
		Flist << "cnn_latest" << "cnn_usa" << "silicon_fr" << "generation_nt";
		Flist << "fdj_loto" << "fdj_euromillions";
		Flist << "horoscope_jour_balance" << "horoscope_jour_belier" << "horoscope_jour_cancer" << "horoscope_jour_capricorne";
		Flist << "horoscope_jour_gemeaux" << "horoscope_jour_lion" << "horoscope_jour_poissons" << "horoscope_jour_sagittaire";
		Flist << "horoscope_jour_scorpion" << "horoscope_jour_taureau" << "horoscope_jour_verseau" << "horoscope_jour_vierge" ;
		SetSettings("ListFlux", Flist);
	}
}

PluginWizzflux::~PluginWizzflux()
{
    Cron::UnregisterAll(this);
}

bool PluginWizzflux::Init()
{
	return true;
}

void PluginWizzflux::OnCron(Bunny * b, QVariant v)
{
	QString name = v.value<QString>();
    streamFlux(b, name);
}

bool PluginWizzflux::OnRFID(Ztamp * z, Bunny * b)
{
	LogInfo(QString("OnRFID zTamp %1 %2").arg(z->GetZtampName(), b->GetBunnyName()));
	QString flux = z->GetPluginSetting(GetName(), QString("Play"), QString()).toString();
	if(flux != "")
	{
		LogInfo(QString("Will now stream from ztamp: %1").arg(flux));
		return streamFlux(b, flux);
	}
	return false;
}

bool PluginWizzflux::OnRFID(Bunny * b, QByteArray const& tag)
{
	LogInfo(QString("OnRFID bunny %1 %2").arg(b->GetBunnyName(), QString(tag.toHex())));
	QString flux = b->GetPluginSetting(GetName(), QString("RFIDPlay/%1").arg(QString(tag.toHex())), QString()).toString();
	if(flux != "")
	{
		LogInfo(QString("Will now stream: %1").arg(flux));
        	return streamFlux(b, flux);
	}
	return false;
}

bool PluginWizzflux::OnClick(Bunny * b, PluginInterface::ClickType)
{
	QString flux = b->GetPluginSetting(GetName(), "DefaultFlux", QString()).toString();
	if(flux.length()) {
		return streamFlux(b, flux);
	}
	return false;
}

void PluginWizzflux::OnBunnyConnect(Bunny * b)
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

void PluginWizzflux::OnBunnyDisconnect(Bunny * b)
{
	Cron::UnregisterAllForBunny(this, b);
}

bool PluginWizzflux::streamFlux(Bunny * b, QString const flux)
{
    QUrl url("http://nabz.wizz.cc/_plugz/");
	url.addEncodedQueryItem("p", flux.toAscii());
	QNetworkAccessManager *manager = new QNetworkAccessManager(this);
	manager->setProperty("BunnyID", b->GetID());
	connect(manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(analyse(QNetworkReply*)));
	manager->get(QNetworkRequest(url));
	//QByteArray message = "ST "+url.toAscii()+"\nPL "+QString::number(qrand() % 8).toAscii()+"\nMW\n";
	return true;
}

void PluginWizzflux::analyse(QNetworkReply* networkReply)
{
	if (!networkReply->error()) {
		Bunny * bunny = BunnyManager::GetBunny(this, networkReply->parent()->property("BunnyID").toByteArray());
		if(bunny) {
            QString message = QString::fromUtf8(networkReply->readAll());
        	if(message != "" && bunny->IsIdle()) {
                message = "ST "+message.toAscii()+"\nPL "+QString::number(qrand() % 8).toAscii()+"\nMW\n";
                bunny->SendPacket(MessagePacket(message.toAscii()));
            }
		}
	}
	networkReply->deleteLater();
	networkReply->parent()->deleteLater();
}

/*******
 * API *
 *******/

void PluginWizzflux::InitApiCalls()
{
	DECLARE_PLUGIN_BUNNY_API_CALL("addrfid(tag,name)", PluginWizzflux, Api_AddRFID);
	DECLARE_PLUGIN_BUNNY_API_CALL("removerfid(tag)", PluginWizzflux, Api_RemoveRFID);
	DECLARE_PLUGIN_BUNNY_API_CALL("addwebcast(time,name)", PluginWizzflux, Api_AddWebcast);
	DECLARE_PLUGIN_BUNNY_API_CALL("removewebcast(time)", PluginWizzflux, Api_RemoveWebcast);
	DECLARE_PLUGIN_BUNNY_API_CALL("setdefault(name)", PluginWizzflux, Api_SetDefault);
	DECLARE_PLUGIN_BUNNY_API_CALL("getdefault()", PluginWizzflux, Api_GetDefault);
	DECLARE_PLUGIN_BUNNY_API_CALL("play(name)", PluginWizzflux, Api_Play);
	DECLARE_PLUGIN_BUNNY_API_CALL("listwebcast()", PluginWizzflux, Api_ListWebcast);
	DECLARE_PLUGIN_BUNNY_API_CALL("listflux()", PluginWizzflux, Api_ListFlux);
	DECLARE_PLUGIN_API_CALL("getflux()", PluginWizzflux, Api_GetFlux);
	DECLARE_PLUGIN_API_CALL("setflux(list)", PluginWizzflux, Api_SetFlux);
}

PLUGIN_BUNNY_API_CALL(PluginWizzflux::Api_AddRFID)
{
	Q_UNUSED(account);

	bunny->SetPluginSetting(GetName(), QString("RFIDPlay/%1").arg(hRequest.GetArg("tag")), hRequest.GetArg("name"));

	return new ApiManager::ApiOk(QString("Add '%1' for RFID '%2', bunny '%3'").arg(hRequest.GetArg("name"), hRequest.GetArg("tag"), QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginWizzflux::Api_RemoveRFID)
{
	Q_UNUSED(account);

	bunny->RemovePluginSetting(GetName(), QString("RFIDPlay/%1").arg(hRequest.GetArg("tag")));

	return new ApiManager::ApiOk(QString("Remove RFID '%2' for bunny '%3'").arg(hRequest.GetArg("tag"), QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginWizzflux::Api_GetDefault)
{
	Q_UNUSED(account);
    Q_UNUSED(hRequest);

	QString name = bunny->GetPluginSetting(GetName(), "DefaultFlux", QString()).toString();
	return new ApiManager::ApiString(name);
}

PLUGIN_BUNNY_API_CALL(PluginWizzflux::Api_SetDefault)
{
	Q_UNUSED(account);

	bunny->SetPluginSetting(GetName(), "DefaultFlux", hRequest.GetArg("name"));
	return new ApiManager::ApiOk(QString("Define '%1' as default for bunny '%2'").arg(hRequest.GetArg("name"), QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginWizzflux::Api_Play)
{
	Q_UNUSED(account);

	if(!bunny->IsIdle())
		return new ApiManager::ApiError(QString("Bunny '%1' is not idle").arg(hRequest.GetArg("to")));

    if(streamFlux(bunny, hRequest.GetArg("name")))
    	return new ApiManager::ApiOk(QString("Now streaming '%1' on bunny '%2'").arg(hRequest.GetArg("name"), QString(bunny->GetID())));
	return new ApiManager::ApiError(QString("Can't stream '%1' on bunny '%2'").arg(hRequest.GetArg("name"), QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginWizzflux::Api_AddWebcast)
{
    Q_UNUSED(account);

    QString hTime = hRequest.GetArg("time");
	if(!bunny->GetPluginSetting(GetName(), "Webcasts", QMap<QString, QVariant>()).toMap().contains(hTime))
    {
        Cron::RegisterDaily(this, QTime::fromString(hTime, "hh:mm"), bunny, QVariant::fromValue(hRequest.GetArg("name")));
	QMap<QString, QVariant> list = bunny->GetPluginSetting(GetName(), "Webcasts", QMap<QString, QVariant>()).toMap();
        list.insert(hTime, hRequest.GetArg("name"));
        bunny->SetPluginSetting(GetName(), "Webcasts", list);
        return new ApiManager::ApiOk(QString("Add webcast at '%1' to bunny '%2'").arg(hRequest.GetArg("time"), QString(bunny->GetID())));
    }
    return new ApiManager::ApiError(QString("Webcast at '%1' already exists for bunny '%2'").arg(hRequest.GetArg("time"), QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginWizzflux::Api_RemoveWebcast)
{
    Q_UNUSED(account);

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

PLUGIN_BUNNY_API_CALL(PluginWizzflux::Api_ListWebcast)
{
	Q_UNUSED(account);
    Q_UNUSED(hRequest);

	QMap<QString, QVariant> list = bunny->GetPluginSetting(GetName(), "Webcasts", QMap<QString, QVariant>()).toMap();

	return new ApiManager::ApiMappedList(list);
}

PLUGIN_BUNNY_API_CALL(PluginWizzflux::Api_ListFlux)
{
	Q_UNUSED(account);
    Q_UNUSED(bunny);
    Q_UNUSED(hRequest);

	return new ApiManager::ApiList(GetSettings("ListFlux", QStringList()).toStringList());
}

PLUGIN_API_CALL(PluginWizzflux::Api_GetFlux)
{
	Q_UNUSED(account);
    Q_UNUSED(hRequest);

	return new ApiManager::ApiList(GetSettings("ListFlux", QStringList()).toStringList());
}

PLUGIN_API_CALL(PluginWizzflux::Api_SetFlux)
{
	if(!account.IsAdmin())
		return new ApiManager::ApiError("Access denied.");

	QStringList list = hRequest.GetArg("list").split(",");
	SetSettings("ListFlux", list);
	Flist = list;
	return new ApiManager::ApiOk("Successfully set flux");
}
