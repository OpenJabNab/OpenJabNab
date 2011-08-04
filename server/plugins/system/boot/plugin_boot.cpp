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
		b->Booting();
	
		LogInfo(QString("Requesting BOOT for tag %1 with version %2").arg(serialnumber,version));

		QString bcFileName = GlobalSettings::Get("Config/Bootcode", "").toString();
		QFile bootcodeFile(bcFileName);
		if(bootcodeFile.open(QFile::ReadOnly))
		{
			QByteArray dataByteArray = bootcodeFile.readAll();
			request.reply = dataByteArray;
			return true;
		}
		else
		{
			LogError("Bootcode not found : " + bcFileName);
			return false;
		}
	}
	else
		return false;
}
