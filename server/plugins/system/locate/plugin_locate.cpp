#include <QDateTime>
#include <QStringList>
#include "plugin_locate.h"
#include "bunny.h"
#include "bunnymanager.h"
#include "log.h"
#include "settings.h"

Q_EXPORT_PLUGIN2(plugin_locate, PluginLocate)

PluginLocate::PluginLocate():PluginInterface("locate", "Manage Locate requests", RequiredPlugin) {}

bool PluginLocate::HttpRequestHandle(HTTPRequest & request)
{
	QString uri = request.GetURI();
	if (uri.startsWith("/vl/locate.jsp"))
	{
		QString serialnumber = request.GetArg("sn").remove(':');
		
		LogInfo(QString("Requesting LOCATE for tag %1").arg(serialnumber));
		
		QString locateString;
		locateString += QString("ping %1\n").arg(GlobalSettings::GetString("OpenJabNabServers/PingServer"));
		locateString += QString("broad %1\n").arg(GlobalSettings::GetString("OpenJabNabServers/BroadServer"));
		locateString += QString("xmpp_domain %1\n").arg(GlobalSettings::GetString("OpenJabNabServers/XmppServer"));
		request.reply = locateString.toAscii();
		return true;
	}
	else
		return false;
}
