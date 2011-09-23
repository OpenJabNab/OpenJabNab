#include <QHttp>
#include <QDebug>
#include <QUrl>
#include <QXmlStreamReader>
#include "bunny.h"
#include "account.h"
#include "plugin_annuaire.h"

Q_EXPORT_PLUGIN2(plugin_annuaire, PluginAnnuaire)

PluginAnnuaire::PluginAnnuaire():PluginInterface("annuaire", "Register the bunny on the central directory", SystemPlugin)
{
}

PluginAnnuaire::~PluginAnnuaire() {}

void PluginAnnuaire::OnBunnyConnect(Bunny * b)
{
	QString server = GetSettings("global/URL", "").toString();
	if(server != "") {
		QHttp *http = new QHttp(server,80);
		http->get("/nabconnection.php?m=" + b->GetID() + "&n="+ b->GetBunnyName() + "&s=" + GlobalSettings::GetString("OpenJabNabServers/PingServer"));
	}
}

QList<BunnyInfos> PluginAnnuaire::SearchBunnyByName(QString name)
{
	QEventLoop loop;

	QHttp *http = new QHttp(GetSettings("global/URL", "").toString(), 80);
	http->get("/whois.php?n=" + QUrl::toPercentEncoding(name));
	QObject::connect(http, SIGNAL(done(bool)), &loop, SLOT(quit()));
	loop.exec();

	QXmlStreamReader xml;
	xml.clear();
	xml.addData(http->readAll());

	QString currentTag;
	QList<BunnyInfos> whois = QList<BunnyInfos>();
	BunnyInfos currentBunny;
	while (!xml.atEnd())
	{
		xml.readNext();
		if (xml.isStartElement() || xml.isEndElement())
		{
			currentTag = xml.name().toString();
		}
		else if (xml.isCharacters() && !xml.isWhitespace())
		{
			if(currentTag == "name")
			{
				currentBunny.name = xml.text().toString();
			}
			else if(currentTag == "macaddress")
			{
				currentBunny.ID = xml.text().toString().toAscii();
			}
			else if(currentTag == "server")
			{
				currentBunny.server = xml.text().toString();
			}
		}
		if(xml.isEndElement() && currentTag == "bunny")
		{
			whois.append(currentBunny);
		}
	}

	return whois;
}

QList<BunnyInfos> PluginAnnuaire::SearchBunnyByMac(QByteArray ID)
{
	QEventLoop loop;

	QHttp *http = new QHttp(GetSettings("global/URL", "").toString(), 80);
	http->get("/whois.php?nm" + QUrl::toPercentEncoding(QString(ID)));
	QObject::connect(http, SIGNAL(done(bool)), &loop, SLOT(quit()));
	loop.exec();

	QXmlStreamReader xml;
	xml.clear();
	xml.addData(http->readAll());

	QString currentTag;
	QList<BunnyInfos> whois = QList<BunnyInfos>();
	BunnyInfos currentBunny;
	while (!xml.atEnd())
	{
		xml.readNext();
		if (xml.isStartElement() || xml.isEndElement())
		{
			currentTag = xml.name().toString();
		}
		else if (xml.isCharacters() && !xml.isWhitespace())
		{
			if(currentTag == "name")
			{
				currentBunny.name = xml.text().toString();
			}
			else if(currentTag == "macaddress")
			{
				currentBunny.ID = xml.text().toString().toAscii();
			}
			else if(currentTag == "server")
			{
				currentBunny.server = xml.text().toString();
			}
		}
		if(xml.isEndElement() && currentTag == "bunny")
		{
			whois.append(currentBunny);
		}
	}

	return whois;
}
/*******/
/* API */
/*******/
void PluginAnnuaire::InitApiCalls()
{
	DECLARE_PLUGIN_API_CALL("setURL(url)", PluginAnnuaire, Api_setURL);
	DECLARE_PLUGIN_API_CALL("getURL()", PluginAnnuaire, Api_getURL);
	DECLARE_PLUGIN_API_CALL("searchbunnybymac(mac)", PluginAnnuaire, Api_SearchBunnyByMac);
	DECLARE_PLUGIN_API_CALL("searchbunnybyname(name)", PluginAnnuaire, Api_SearchBunnyByName);
}

PLUGIN_API_CALL(PluginAnnuaire::Api_setURL)
{
	if(!account.IsAdmin())
		return new ApiManager::ApiError("Access denied");

	QString url = hRequest.GetArg("url");
	SetSettings("global/URL", url);

	return new ApiManager::ApiOk(QString("URL set to '%1'.").arg(url));
}

PLUGIN_API_CALL(PluginAnnuaire::Api_getURL)
{
	Q_UNUSED(hRequest);

	if(!account.IsAdmin())
		return new ApiManager::ApiError("Access denied");

	return new ApiManager::ApiString(QString("%1").arg(GetSettings("global/URL", "").toString()));
}

PLUGIN_API_CALL(PluginAnnuaire::Api_SearchBunnyByMac)
{
	Q_UNUSED(account);

	QList<BunnyInfos> whois = SearchBunnyByMac(hRequest.GetArg("mac").toAscii());
	QString xml = "";
	foreach(BunnyInfos b, whois)
	{
		xml += "<bunny>";
		xml += "<name>" + b.name + "</name>";
		xml += "<ID>" + b.ID + "</ID>";
		xml += "<server>" + b.server + "</server>";
		xml += "</bunny>\n";
	}
	return new ApiManager::ApiXml(xml);
}

PLUGIN_API_CALL(PluginAnnuaire::Api_SearchBunnyByName)
{
	Q_UNUSED(account);

	QList<BunnyInfos> whois = SearchBunnyByName(hRequest.GetArg("name"));
	QString xml = "";
	foreach(BunnyInfos b, whois)
	{
		xml += "<bunny>";
		xml += "<name>" + b.name + "</name>";
		xml += "<ID>" + b.ID + "</ID>";
		xml += "<server>" + b.server + "</server>";
		xml += "</bunny>\n";
	}
	return new ApiManager::ApiXml(xml);
}
