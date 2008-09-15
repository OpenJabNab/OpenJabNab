#include <QDateTime>
#include <QStringList>
#include "plugin_boot.h"
#include "bunny.h"
#include "bunnymanager.h"
#include "log.h"
#include "settings.h"

Q_EXPORT_PLUGIN2(plugin_boot, PluginBoot)

PluginBoot::PluginBoot():PluginInterface("boot", "Boot", RequiredPlugin) {}

bool PluginBoot::HttpRequestHandle(HTTPRequest & request)
{
	QString uri = request.GetURI();
	if (uri.startsWith("/vl/bc.jsp"))
	{
		QString version = request.GetArg("v");
		QString serialnumber = request.GetArg("m").remove(':');
		
		Bunny * b = BunnyManager::GetBunny(serialnumber.toAscii());
		b->SetGlobalSetting("Last BootRequest", QDateTime::currentDateTime());
		
		Log::Info(QString("Requesting BOOT for tag %1 with version %2").arg(serialnumber,version));
		request.reply = request.ForwardTo(GlobalSettings::GetString("DefaultVioletServers/BootServer"));
		return true;
		
	}
	else
		return false;
}
