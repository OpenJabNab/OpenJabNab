#include <QDateTime>
#include <QStringList>
#include "plugin_rfid.h"
#include "bunny.h"
#include "bunnymanager.h"
#include "ztamp.h"
#include "ztampmanager.h"
#include "log.h"
#include "settings.h"

Q_EXPORT_PLUGIN2(plugin_rfid, PluginRFID)

PluginRFID::PluginRFID():PluginInterface("rfid", "Manage RFID requests", RequiredPlugin) {}

bool PluginRFID::HttpRequestHandle(HTTPRequest & request)
{
	QString uri = request.GetURI();
	if (uri.startsWith("/vl/rfid.jsp"))
	{
		QString serialnumber = request.GetArg("sn");
		QString tagId = request.GetArg("t");

		Ztamp * z = ZtampManager::GetZtamp(this, tagId.toAscii());
		Bunny * b = BunnyManager::GetBunny(this, serialnumber.toAscii());
	
		if (z->OnRFID(b))
			return true;
		if (b->OnRFID(QByteArray::fromHex(tagId.toAscii())))
			return true;
	}
	return false;
}
