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
	availableColors << "cyan" << "yellow" << "green" << "red" << "violet" << "blue" << "white" << "none";
}

void PluginColorbreathing::patchBootcode(HTTPRequest & request, long address, char origin, char patch)
{
	QByteArray origin_;
	QByteArray patch_;
	origin_.append(origin);
	patch_.append(patch);
	patchBootcode(request, address, 1, origin_, patch_);
}

void PluginColorbreathing::patchBootcode(HTTPRequest & request, long address, int size, QByteArray origin, QByteArray patch)
{
	if(request.reply.indexOf(origin, address) != address)
	{
		LogDebug("Firmware can't be patched");
	}
	else
	{
		LogDebug("Patching firmware");
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

			if(color == "violet")
			{
				LogDebug(QString("No color change for bunny '%1'").arg(QString(b->GetID())));
			}
			else if(color == "none")
			{
				LogDebug(QString("Applying color '%1' for bunny '%2'").arg(color, QString(b->GetID())));
				patchBootcode(request, 0x000183D8, (char)0x01, (char)0x00); 
				patchBootcode(request, 0x000183DA, (char)0x01, (char)0x00); 
			}
			else if(color == "yellow")
			{
				LogDebug(QString("Applying color '%1' for bunny '%2'").arg(color, QString(b->GetID())));
				patchBootcode(request, 0x000183D8, (char)0x01, (char)0x00); 
				patchBootcode(request, 0x000183D9, (char)0x00, (char)0x01);
			}
			else if(color == "cyan")
			{
				LogDebug(QString("Applying color '%1' for bunny '%2'").arg(color, QString(b->GetID())));
				patchBootcode(request, 0x000183D9, (char)0x00, (char)0x01);
				patchBootcode(request, 0x000183DA, (char)0x01, (char)0x00); 
			}
			else if(color == "blue")
			{
				LogDebug(QString("Applying color '%1' for bunny '%2'").arg(color, QString(b->GetID())));
				patchBootcode(request, 0x000183DA, (char)0x01, (char)0x00); 
			}
			else if(color == "red")
			{
				LogDebug(QString("Applying color '%1' for bunny '%2'").arg(color, QString(b->GetID())));
				patchBootcode(request, 0x000183D8, (char)0x01, (char)0x00); 
			}
			else if(color == "green")
			{
				LogDebug(QString("Applying color '%1' for bunny '%2'").arg(color, QString(b->GetID())));
				patchBootcode(request, 0x000183D8, (char)0x01, (char)0x00); 
				patchBootcode(request, 0x000183D9, (char)0x00, (char)0x01);
				patchBootcode(request, 0x000183DA, (char)0x01, (char)0x00); 
			}
			else if(color == "white")
			{
				LogDebug(QString("Applying color '%1' for bunny '%2'").arg(color, QString(b->GetID())));
				patchBootcode(request, 0x000183D9, (char)0x00, (char)0x01);
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

	return new ApiManager::ApiList(availableColors);
}

