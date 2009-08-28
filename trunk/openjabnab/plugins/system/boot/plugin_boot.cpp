#include <QDateTime>
#include <QStringList>
#include "plugin_boot.h"
#include "bunny.h"
#include "bunnymanager.h"
#include "log.h"
#include "settings.h"

Q_EXPORT_PLUGIN2(plugin_boot, PluginBoot)

PluginBoot::PluginBoot():PluginInterface("boot", "Manage Boot requests", RequiredPlugin) {}

bool PluginBoot::HttpRequestHandle(HTTPRequest & request)
{
	QString uri = request.GetURI();
	if (uri.startsWith("/vl/bc.jsp"))
	{
		QString version = request.GetArg("v");
		QString serialnumber = request.GetArg("m").remove(':');
		
		Bunny * b = BunnyManager::GetBunny(this, serialnumber.toAscii());
		b->SetGlobalSetting("Last BootRequest", QDateTime::currentDateTime());
		
		Log::Info(QString("Requesting BOOT for tag %1 with version %2").arg(serialnumber,version));
		if(GlobalSettings::Get("Config/StandAlone", true) == false)
		{
			request.reply = request.ForwardTo(GlobalSettings::GetString("DefaultVioletServers/BootServer"));
			return true;
		}
		else
		{
			QFile bootcodeFile(GlobalSettings::Get("Config/Bootcode", "").toString());
			if(bootcodeFile.open(QFile::ReadOnly))
			{
				QByteArray dataByteArray = bootcodeFile.readAll();
				request.reply = dataByteArray;
				return true;
			}
		}
		return false;
	}
	else
		return false;
}
