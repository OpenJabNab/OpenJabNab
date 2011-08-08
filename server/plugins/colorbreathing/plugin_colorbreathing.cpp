#include <QDateTime>
#include <QStringList>
#include "plugin_colorbreathing.h"
#include "bunny.h"
#include "bunnymanager.h"
#include "log.h"
#include "settings.h"

Q_EXPORT_PLUGIN2(plugin_colorbreathing, PluginColorbreathing)

PluginColorbreathing::PluginColorbreathing():PluginInterface("colorbreathing", "Change breathing color", BunnyPlugin)
{
	availableColors["cyan"]   = QByteArray("\x01\x01\x00",3);
	availableColors["yellow"] = QByteArray("\x00\x01\x01",3);
	availableColors["green"]  = QByteArray("\x00\x01\x00",3);
	availableColors["red"]    = QByteArray("\x00\x00\x01",3);
	availableColors["violet"] = QByteArray("\x01\x00\x01",3);
	availableColors["blue"]   = QByteArray("\x01\x00\x00",3);
	availableColors["white"]  = QByteArray("\x01\x01\x01",3);
	availableColors["none"]   = QByteArray("\x00\x00\x00",3);
}

void PluginColorbreathing::patchBootcode(HTTPRequest & request, long address, int size, QByteArray origin, QByteArray patch)
{
	if(request.reply.indexOf(origin, address) != address)
	{
		LogDebug("Firmware can't be patched for colorbreathing");
	}
	else
	{
		request.reply.replace(address, size, patch);
	}
}

void PluginColorbreathing::HttpRequestAfter(HTTPRequest & request)
{
	QString uri = request.GetURI();
	if (uri.startsWith("/vl/bc.jsp"))
	{
                QString version = request.GetArg("v");
                QString serialnumber = request.GetArg("m").remove(':');
                
        	Bunny * b = BunnyManager::GetBunny(this, serialnumber.toAscii());
		if(b)
		{
			QString color = b->GetPluginSetting(GetName(), "color", QString("violet")).toString();
			if(availableColors.contains(color))
			{
				patchBootcode(request, 0x000183D8, 3, QByteArray("\x01\x00\x01",3), availableColors.value(color));
			}
		}
	}
}

void PluginColorbreathing::InitApiCalls()
{
        DECLARE_PLUGIN_BUNNY_API_CALL("getColorList()", PluginColorbreathing, Api_GetColorList);
        DECLARE_PLUGIN_BUNNY_API_CALL("setColor(name)", PluginColorbreathing, Api_SetColor);
        DECLARE_PLUGIN_BUNNY_API_CALL("getColor()", PluginColorbreathing, Api_GetColor);
}

PLUGIN_BUNNY_API_CALL(PluginColorbreathing::Api_GetColor)
{
        Q_UNUSED(account);
	Q_UNUSED(hRequest);

        return new ApiManager::ApiOk(bunny->GetPluginSetting(GetName(), "color", QString("violet")).toString());
}

PLUGIN_BUNNY_API_CALL(PluginColorbreathing::Api_SetColor)
{
        Q_UNUSED(account);

        QString color = hRequest.GetArg("name");
        if(availableColors.contains(color))
        {
                // Save new config
                bunny->SetPluginSetting(GetName(), "color", color);

                return new ApiManager::ApiOk(QString("color changed to '%1'").arg(color));
        }
        return new ApiManager::ApiError(QString("Unknown '%1' color").arg(color));
}

PLUGIN_BUNNY_API_CALL(PluginColorbreathing::Api_GetColorList)
{
	Q_UNUSED(account);
	Q_UNUSED(bunny);
	Q_UNUSED(hRequest);

	return new ApiManager::ApiList(availableColors.keys());
}

