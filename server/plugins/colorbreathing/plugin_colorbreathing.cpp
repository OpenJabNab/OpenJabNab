#include <QDateTime>
#include <QStringList>
#include "plugin_colorbreathing.h"
#include "ambientpacket.h"
#include "bunny.h"
#include "bunnymanager.h"
#include "log.h"
#include "settings.h"

Q_EXPORT_PLUGIN2(plugin_colorbreathing, PluginColorbreathing)

PluginColorbreathing::PluginColorbreathing():PluginInterface("colorbreathing", "Change breathing color", BunnyPlugin)
{
	availableColors["none"]   = 0;
	availableColors["blue"]   = 1;
	availableColors["green"]  = 2;
	availableColors["cyan"]   = 3;
	availableColors["red"]    = 4;
	availableColors["violet"] = 5;
	availableColors["yellow"] = 6;
	availableColors["white"]  = 7;
}

void PluginColorbreathing::OnInitPacket(const Bunny * bunny, AmbientPacket & a, SleepPacket &)
{
	QString color = bunny->GetPluginSetting(GetName(), "color", QString("violet")).toString();
	a.SetServiceValue(AmbientPacket::Service_BottomLed, availableColors[color]);
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

		// Send color to bunny
		bunny->SendPacket(AmbientPacket(AmbientPacket::Service_BottomLed, availableColors[color]));

		return new ApiManager::ApiOk(QString("Bottom Color set to '%1'").arg(color));
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

