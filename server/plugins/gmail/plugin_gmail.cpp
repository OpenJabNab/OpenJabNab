#include "plugin_gmail.h"
#include "cron.h"
#include <QMapIterator>
#include "bunny.h"
#include "bunnymanager.h"
#include "messagepacket.h"
#include "ambientpacket.h"
#include "ttsmanager.h"
#include "log.h"
#include <QHttp>


Q_EXPORT_PLUGIN2(plugin_gmail, PluginGmail)

PluginGmail::PluginGmail():PluginInterface("gmail", "Gmail Configuration",BunnyPlugin)
{
}

PluginGmail::~PluginGmail()
{
Cron::UnregisterAll(this);
}

void PluginGmail::OnBunnyConnect(Bunny * b)
{
int frequency = b->GetPluginSetting(GetName(), "Frequency", 0).toInt();
if(frequency < 1)
frequency = 1;
if(frequency > 500)
frequency = 500;
Cron::Register(this, frequency, 0, 0, b, QVariant( (int) NULL) );
}

void PluginGmail::OnBunnyDisconnect(Bunny * b)
{
Cron::UnregisterAllForBunny(this, b);

}

void PluginGmail::OnCron(Bunny * b, QVariant)
{
QMapIterator<Bunny *, QString> i(bunnyList);
QString email = b->GetPluginSetting(GetName(), QString("Email"), QString()).toString();
QString password = b->GetPluginSetting(GetName(), QString("Password"), QString()).toString();


    connect(&http, SIGNAL(readyRead(QHttpResponseHeader)),this, SLOT(readData(QHttpResponseHeader)));
    xml.clear();

    if((email != "") && (password != ""))
{
    http.setHost("mail.google.com", QHttp::ConnectionModeHttps);
    http.setUser(email, password);
    http.setProperty("BunnyID", b->GetID());
    http.get("/mail/feed/atom");
}

return;
}


void PluginGmail::readData(const QHttpResponseHeader &resp)
{
Bunny * bunny = BunnyManager::GetBunny(this, http.property("BunnyID").toByteArray());

    if (resp.statusCode() != 200)
    {
http.abort();
LogWarning("Connection error");
    }
    else {
xml.addData(http.readAll());

while (!xml.atEnd()) {
xml.readNext();
if (xml.isStartElement()) {
currentTag = xml.name().toString();
} else if (xml.isCharacters() && !xml.isWhitespace()) {
if (currentTag == "fullcount")
{
bool ok;
emailsCount = xml.text().toString().toInt(&ok);
if (emailsCount > 0)
{
bunny->SendPacket(AmbientPacket(AmbientPacket::Service_Nose, 1));

}
else
{
bunny->SendPacket(AmbientPacket(AmbientPacket::Service_Nose, 0));

}
}
}
    }
}
}


/*******
* API *
*******/

void PluginGmail::InitApiCalls()
{
DECLARE_PLUGIN_BUNNY_API_CALL("setConfig()", PluginGmail, Api_SetConfig);
DECLARE_PLUGIN_BUNNY_API_CALL("getConfig()", PluginGmail, Api_GetConfig);
}



PLUGIN_BUNNY_API_CALL(PluginGmail::Api_SetConfig)
{
Q_UNUSED(account);


if(!hRequest.HasArg("Frequency"))
return new ApiManager::ApiError(QString("Missing argument 'Frequency' for plugin Gmail"));
if(!hRequest.HasArg("Email"))
return new ApiManager::ApiError(QString("Missing argument 'Email' for plugin Gmail"));
if(!hRequest.HasArg("Password"))
return new ApiManager::ApiError(QString("Missing argument 'Password' for plugin Gmail"));

bunny->SetPluginSetting(GetName(), "Frequency", hRequest.GetArg("Frequency"));
bunny->SetPluginSetting(GetName(), "Email", hRequest.GetArg("Email"));
bunny->SetPluginSetting(GetName(), "Password", hRequest.GetArg("Password"));

return new ApiManager::ApiOk(QString("Account '%1' will be verified every '%2' minute(s) for bunny '%3'").arg(hRequest.GetArg("Email"),hRequest.GetArg("Frequency"), QString(bunny->GetID())));
}

PLUGIN_BUNNY_API_CALL(PluginGmail::Api_GetConfig)
{
    Q_UNUSED(account);
Q_UNUSED(hRequest);

QList<QString> list;
list.append(bunny->GetPluginSetting(GetName(), "Frequency", QString()).toString());
list.append(bunny->GetPluginSetting(GetName(), "Email", QString()).toString());
list.append(bunny->GetPluginSetting(GetName(), "Password", QString()).toString());

return new ApiManager::ApiList(list);
}
