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
#include "plugin_ratp.h"
#include "settings.h"
#include "ttsmanager.h"


Q_EXPORT_PLUGIN2(plugin_ratp, PluginRatp)

PluginRatp::PluginRatp():PluginInterface("ratp", "RATP : Prochains passages")
{
}

PluginRatp::~PluginRatp() {}

void PluginRatp::OnBunnyConnect(Bunny *)
{
}

void PluginRatp::OnBunnyDisconnect(Bunny *)
{
}

void PluginRatp::OnInitPacket(const Bunny *, AmbientPacket &, SleepPacket &)
{
}

bool PluginRatp::OnClick(Bunny * b, PluginInterface::ClickType type)
{
	if (type == PluginInterface::SingleClick) {
		
		QString str = b->GetPluginSetting(GetName(), "Default/Arret", "").toString();
		QString ligne;
		QString arret;
		QString direction;
		QString reseau;
		int pos = 0;

		LogDebug(str);
		QRegExp rx("([A-Za-z0-9]*)\\|([A-Za-z0-9]*)\\|([A-Za-z0-9]*)\\|([A-Za-z0-9_]*)");
		if(rx.indexIn(str, pos) != -1){
			ligne = rx.cap(2);
			arret = rx.cap(3);
			direction = rx.cap(4);
			reseau = rx.cap(1);
		}
		
		if(ligne != "" && arret != "" && direction != "" && reseau != "") {
			getPageHoraire(b, reseau, ligne, arret, direction);
			return true;
		}
	}
	return false;
}

void PluginRatp::OnCron(Bunny * b, QVariant v)
{
	QString str = v.value<QString>();
	QString ligne;
	QString arret;
	QString direction;
	QString reseau;
	int pos = 0;
	QRegExp rx("([A-Za-z0-9]*)\\|([A-Za-z0-9]*)\\|([A-Za-z0-9]*)\\|([A-Za-z0-9_]*)");
	if(rx.indexIn(str, pos) != -1){
		ligne = rx.cap(2);
		arret = rx.cap(3);
		direction = rx.cap(4);
		reseau = rx.cap(1);
	}
	
	if(ligne != "" && arret != "" && direction != "" && reseau != "") {
		getPageHoraire(b, reseau, ligne, arret, direction);
	}
}

bool PluginRatp::OnRFID(Bunny * b, QByteArray const& tag)
{
	QString str = b->GetPluginSetting(GetName(), QString("RFIDRatp/%1").arg(QString(tag.toHex())), QString()).toString();
	QString ligne;
	QString arret;
	QString direction;
	QString reseau;
	LogDebug("RFID");
	int pos = 0;
	QRegExp rx("([A-Za-z0-9]*)\\|([A-Za-z0-9]*)\\|([A-Za-z0-9]*)\\|([A-Za-z0-9_]*)");
	if(rx.indexIn(str, pos) != -1){
		ligne = rx.cap(2);
		arret = rx.cap(3);
		direction = rx.cap(4);
		reseau = rx.cap(1);
	}
	
	if(ligne != "" && arret != "" && direction != "" && reseau != "") {
		getPageHoraire(b, reseau, ligne, arret, direction);
		return true;
	}
	return false;
}
void PluginRatp::getPageHoraire(Bunny * b,QString reseau, QString ligne, QString arret, QString direction)
{
	Q_UNUSED(b);

	QString sb = QString("http://wap.ratp.fr/siv/schedule?service=next&reseau=%1&lineid=%2&directionsens=%3&stationid=%4").arg(reseau,ligne,direction,arret);
	QUrl url(sb);
	QNetworkAccessManager *manager = new QNetworkAccessManager(this);
	manager->setProperty("BunnyID", b->GetID());
	connect(manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(analyseXml(QNetworkReply*)));
	manager->get(QNetworkRequest(url));
}

void PluginRatp::analyseXml(QNetworkReply* networkReply)
{
	if (!networkReply->error()) {
		Bunny * bunny = BunnyManager::GetBunny(this, networkReply->parent()->property("BunnyID").toByteArray());
		if(bunny) {
			PluginRatp_Worker * p = new PluginRatp_Worker(this, bunny, QString::fromUtf8(networkReply->readAll()));
			connect(p, SIGNAL(done(bool,Bunny*,QByteArray)), this, SLOT(analyseDone(bool,Bunny*,QByteArray)));
			connect(p, SIGNAL(finished()), p, SLOT(deleteLater()));
			p->start();
		}
	}
	networkReply->deleteLater();
	networkReply->parent()->deleteLater();
}


void PluginRatp::analyseDone(bool ret, Bunny * b, QByteArray message)
{
	if(ret && b && b->IsIdle())
		 b->SendPacket(MessagePacket(message));
}

void PluginRatp::InitApiCalls()
{
	DECLARE_PLUGIN_BUNNY_API_CALL("addarret(item)", PluginRatp, Api_addArret);
	DECLARE_PLUGIN_BUNNY_API_CALL("removearret(item)", PluginRatp, Api_removeArret);
	DECLARE_PLUGIN_BUNNY_API_CALL("getlistarret()", PluginRatp, Api_getListArret);
	DECLARE_PLUGIN_BUNNY_API_CALL("setdefaultarret(item)", PluginRatp, Api_setDefaultArret);
	DECLARE_PLUGIN_BUNNY_API_CALL("getdefaultarret()", PluginRatp, Api_getArretDefaut);
	DECLARE_PLUGIN_BUNNY_API_CALL("addwebcast(time,item)", PluginRatp, Api_AddWebcast);
	DECLARE_PLUGIN_BUNNY_API_CALL("removewebcast(time)", PluginRatp, Api_RemoveWebcast);
	DECLARE_PLUGIN_BUNNY_API_CALL("getwebcastslist()", PluginRatp, Api_ListWebcast);
	DECLARE_PLUGIN_BUNNY_API_CALL("addrfid(tag,item)", PluginRatp, Api_AddRFID);
	DECLARE_PLUGIN_BUNNY_API_CALL("removerfid(tag)", PluginRatp, Api_RemoveRFID);
}


PLUGIN_BUNNY_API_CALL(PluginRatp::Api_addArret) {
	Q_UNUSED(account);

	if(!hRequest.HasArg("item"))
		return new ApiManager::ApiError(QString("Missing something for plugin Ratp"));

	QString tmp = hRequest.GetArg("item");
	
	QStringList list = bunny->GetPluginSetting(GetName(), "Arrets", QStringList()).toStringList();
	list.append(tmp);
	bunny->SetPluginSetting(GetName(), "Arrets", list);

	return new ApiManager::ApiOk(QString("Added arrets '%1' for bunny '%2'").arg(tmp, QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginRatp::Api_getListArret) {
	Q_UNUSED(account);
	Q_UNUSED(hRequest);
	return new ApiManager::ApiList(bunny->GetPluginSetting(GetName(), "Arrets", QStringList()).toStringList());
}

PLUGIN_BUNNY_API_CALL(PluginRatp::Api_setDefaultArret)
{
	Q_UNUSED(account);

	if(!hRequest.HasArg("item"))
		return new ApiManager::ApiError(QString("Missing something for plugin Ratp"));

	QString tmp = hRequest.GetArg("item");

	bunny->SetPluginSetting(GetName(), "Default/Arret", tmp);
	return new ApiManager::ApiOk(QString("Added default arrets '%1' for bunny '%2'").arg(tmp, QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginRatp::Api_getArretDefaut)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);

	return new ApiManager::ApiString(bunny->GetPluginSetting(GetName(), "Default/Arret",QString()).toString());
}

PLUGIN_BUNNY_API_CALL(PluginRatp::Api_removeArret)
{
	Q_UNUSED(account);

	if(!hRequest.HasArg("item"))
		return new ApiManager::ApiError(QString("Missing argument 'item' for plugin Ratp"));

	QString item = hRequest.GetArg("item");
	QStringList list = bunny->GetPluginSetting(GetName(), "Arrets", QStringList()).toStringList();
	list.removeAll(item);
	bunny->SetPluginSetting(GetName(), "Arrets", list);

	return new ApiManager::ApiOk(QString("Removed arret '%1' for bunny '%2'").arg(item, QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginRatp::Api_AddWebcast)
{
	Q_UNUSED(account);

	QString hTime = hRequest.GetArg("time");
	QString item = hRequest.GetArg("item");
	QMap<QString, QVariant> list = bunny->GetPluginSetting(GetName(), "Webcasts", QMap<QString, QVariant>()).toMap();
	if(!list.contains(hTime))
	{
		Cron::RegisterDaily(this, QTime::fromString(hTime, "hh:mm"), bunny, QVariant::fromValue(item));
		list.insert(hTime,item);
		bunny->SetPluginSetting(GetName(), "Webcasts", list);
		return new ApiManager::ApiOk(QString("Add webcast at '%1' to bunny '%2'").arg(hTime, QString(bunny->GetID())));
	}
	return new ApiManager::ApiError(QString("Webcast already exists at '%1' for bunny '%2'").arg(hTime, QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginRatp::Api_RemoveWebcast)
{
	Q_UNUSED(account);

	if(!hRequest.HasArg("time"))
		return new ApiManager::ApiError(QString("Missing argument 'time' for plugin Ratp"));

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

PLUGIN_BUNNY_API_CALL(PluginRatp::Api_ListWebcast)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);
	return new ApiManager::ApiMappedList(bunny->GetPluginSetting(GetName(), "Webcasts", QMap<QString, QVariant>()).toMap());
}

PLUGIN_BUNNY_API_CALL(PluginRatp::Api_AddRFID)
{
	Q_UNUSED(account);

	bunny->SetPluginSetting(GetName(), QString("RFIDRatp/%1").arg(hRequest.GetArg("tag")), hRequest.GetArg("item"));

	return new ApiManager::ApiOk(QString("Add Ratp for '%1' for RFID '%2', bunny '%3'").arg(hRequest.GetArg("item"), hRequest.GetArg("tag"), QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginRatp::Api_RemoveRFID)
{
	Q_UNUSED(account);

	bunny->RemovePluginSetting(GetName(), QString("RFIDRatp/%1").arg(hRequest.GetArg("tag")));

	return new ApiManager::ApiOk(QString("Remove RFID '%2' for bunny '%3'").arg(hRequest.GetArg("tag"), QString(bunny->GetID())));
}

/* WORKER THREAD */
PluginRatp_Worker::PluginRatp_Worker(PluginRatp * p, Bunny * bu, QString b):plugin(p),bunny(bu),buffer(b.replace("&amp;", "and").replace("</script>","").replace("&gt;","").replace("&nbsp;","").replace("&eacute;", "e").replace("&eagrave;", "e").replace("&ecirc;", "e"))
{

}

void PluginRatp_Worker::run()
{
	bool noError = false;

	int type = 0; // Gare = 1, Station|Arrêt = 2;
	QString currentTag;
	QString chaine;
	QByteArray message;

	QString sPassingTime;
	QStringList sDirection;
	QString sArret;
	QStringList sProchainPassages;
	
	QString bla = QString(buffer);
	bla = bla.simplified();
	
	QRegExp rx3("(Gare|Station|Arret) <b class=\"bwhite\">([A-Za-z0-9 :-+']*)+</b>");
	
	QRegExp rx2 ("<div class=\"bg[1|3]\">([A-Za-z0-9 :-+']*)+</div>");
	QRegExp rx ("<div class=\"schmsg[1-3]\"><b>([A-Za-z0-9 :]*)+</b></div>");
	
 	QStringList list;
 	int pos = 0;

	if(rx3.indexIn(bla, pos) != -1){
		if(rx3.cap(1) == "Gare")
			type = 1;
		else
			type = 2;
		sArret = rx3.cap(2);
	}

 	while ((pos = rx2.indexIn(bla, pos)) != -1) {
		noError = false;
     	sDirection << rx2.cap(1);
		pos += rx2.matchedLength();
 	}
	pos = 0;
 	while ((pos = rx.indexIn(bla, pos)) != -1) {
		noError = false;
     	sProchainPassages << rx.cap(1);
		pos += rx.matchedLength();
 	}	

	if (noError == true)
	{
		LogDebug(QString("Error: %1").arg("xml.errorString()"));
		emit done(false, bunny, QByteArray());
	}
	else
	{
		if(pos == 0)
		{
			emit done(false, bunny, QByteArray());
		}
		else
		{
			QByteArray where = TTSManager::CreateNewSound(QString("Prochain passage a %1").arg(sArret), "claire");
			message += "MU "+where+"\nPL 3\nMW\n";
			if(pos!=0)
			{
				for (int i = 0; i < sProchainPassages.size() && i < sDirection.size(); ++i){
					QByteArray direction = TTSManager::CreateNewSound(QString("Direction %1").arg(sDirection.at(i)), "claire");
					message += "MU "+direction+"\nPL 3\nMW\n";
					if(type == 1){
						QByteArray passage = TTSManager::CreateNewSound(QString("passera a %1").arg(sProchainPassages.at(i)), "claire");
						message += "MU "+passage+"\nPL 3\nMW\n";
					}else{
						QByteArray passage = TTSManager::CreateNewSound(QString("passera dans %1").arg(sProchainPassages.at(i)), "claire");						
						message += "MU "+passage+"\nPL 3\nMW\n";
					}

				}
			} 
			emit done(true, bunny, message);
		}
	}
}
