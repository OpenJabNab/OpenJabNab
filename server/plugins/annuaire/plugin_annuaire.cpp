#include <QHttp>
#include "bunny.h"
#include "plugin_annuaire.h"

Q_EXPORT_PLUGIN2(plugin_annuaire, PluginAnnuaire)

PluginAnnuaire::PluginAnnuaire():PluginInterface("annuaire", "Register the bunny on the annuaire", RequiredPlugin)
{
}

PluginAnnuaire::~PluginAnnuaire() {}

void PluginAnnuaire::OnBunnyConnect(Bunny * b)
{
	QHttp *http = new QHttp("www.freenabaztag.com",80);
	http->get("/nabconnection.php?m=" + b->GetID() + "&n="+ b->GetBunnyName() + "&s=" + GlobalSettings::GetString("OpenJabNabServers/PingServer"));
}
