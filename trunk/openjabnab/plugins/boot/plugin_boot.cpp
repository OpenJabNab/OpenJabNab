#include <QStringList>

#include "settings.h"
#include "plugin_boot.h"
#include "log.h"

Q_EXPORT_PLUGIN2(plugin_boot, PluginBOOT)

bool PluginBOOT::HttpRequestHandle(HTTPRequest & request)
{
	QString uri = request.GetURI();
	if (uri.startsWith("/vl/bc.jsp"))
	{
		QStringList args = request.GetArgs();
		QString version;
		QString serialnumber;
		
		foreach(QString arg, args)
		{
			if (arg.startsWith("v="))
				version = arg.remove(0,2);
			else if (arg.startsWith("m="))
				serialnumber = arg.remove(0,2).remove(':');
		}
		
		Log::Info(QString("Requesting BOOT for tag %1 with version %2").arg(serialnumber,version));
		request.reply = request.ForwardTo(GlobalSettings::GetString("DefaultVioletServers/BootServer"));
		return true;
		
	}
	else
		return false;
}
