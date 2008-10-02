#include <QDateTime>
#include <QStringList>
#include "plugin_locate.h"
#include "bunny.h"
#include "bunnymanager.h"
#include "log.h"
#include "settings.h"

Q_EXPORT_PLUGIN2(plugin_locate, PluginLocate)

PluginLocate::PluginLocate():PluginInterface("locate", "Locate", RequiredPlugin) {}

bool PluginLocate::HttpRequestHandle(HTTPRequest & request)
{
	QString uri = request.GetURI();
	if (uri.startsWith("/vl/locate.jsp"))
	{
		QString serialnumber = request.GetArg("sn").remove(':');
		
		Bunny * b = BunnyManager::GetBunny(serialnumber.toAscii());
		b->SetGlobalSetting("Last LocateRequest", QDateTime::currentDateTime());
		
		Log::Info(QString("Requesting LOCATE for tag %1").arg(serialnumber));
		
		// Forward request to Violet
		QByteArray violetAnswer = request.ForwardTo(GlobalSettings::GetString("DefaultVioletServers/BootServer"));

		// Analyse the answer to see if servers has changed
		QList<QByteArray> lines = violetAnswer.split('\n');
		foreach(QByteArray line, lines)
		{
			if (line.startsWith("ping")) 
			{}
			else if (line.startsWith("broad"))
			{}
			else if (line.startsWith("xmpp_domain"))
			{}
			else
				if (line.size() != 0)
					Log::Error(QString("Unknown locate server : ") + line);
		}

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
