#include <QDateTime>
#include <QStringList>
#include "plugin_rfid.h"
#include "bunny.h"
#include "bunnymanager.h"
#include "log.h"
#include "pluginmanager.h"
#include "settings.h"

Q_EXPORT_PLUGIN2(plugin_rfid, PluginRFID)

PluginRFID::PluginRFID():PluginInterface("rfid", "RFID", RequiredPlugin) {}

bool PluginRFID::HttpRequestHandle(HTTPRequest & request)
{
	QString uri = request.GetURI();
	if (uri.startsWith("/vl/rfid.jsp"))
	{
		QString serialnumber = request.GetArg("sn");
		QString tagId = request.GetArg("t");

		Bunny * b = BunnyManager::GetBunny(serialnumber.toAscii());
		b->SetGlobalSetting("Last RFID Tag", tagId);
	
		if (PluginManager::Instance().OnRFID(b, QByteArray::fromHex(tagId.toAscii())))
			return true;
	}
	return false;
}
