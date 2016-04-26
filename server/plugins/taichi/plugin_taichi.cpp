#include "plugin_taichi.h"
#include "ambientpacket.h"
#include "taichipacket.h"
#include "bunny.h"

Q_EXPORT_PLUGIN2(plugin_taichi, PluginTaichi)

PluginTaichi::PluginTaichi():PluginInterface("taichi", "Manage Bunny's Taichi", BunnyPlugin)
{
}

PluginTaichi::~PluginTaichi() {}

void PluginTaichi::OnBunnyConnect(Bunny * b)
{
	SendTaichiFrequency(b);
}

void PluginTaichi::OnBunnyDisconnect(Bunny * b)
{
	if(b->IsConnected())
		b->SendPacket(AmbientPacket(AmbientPacket::Service_TaiChi,0));
}

void PluginTaichi::OnInitPacket(const Bunny * b, AmbientPacket & a, SleepPacket &)
{
	int frequency = b->GetPluginSetting(GetName(), "frequency", 0).toInt();
	if(frequency < 0)
		frequency = 0;
	if(frequency > 255)
		frequency = 255;
	a.SetServiceValue(AmbientPacket::Service_TaiChi,frequency);
}

void PluginTaichi::SendTaichiFrequency(Bunny * b)
{
	int frequency = b->GetPluginSetting(GetName(), "frequency", 0).toInt();
	if(frequency < 0)
		frequency = 0;
	if(frequency > 255)
		frequency = 255;
	b->SendPacket(AmbientPacket(AmbientPacket::Service_TaiChi,frequency));
}

bool PluginTaichi::OnClick(Bunny * b, PluginInterface::ClickType type)
{
	if (type == PluginInterface::SingleClick)
	{
		if(b->GetPluginSetting(GetName(), "taichiOnClick", false).toBool())
		{
			b->SendPacket(TaichiPacket());
		        return true;
		}
        }
	return false;
}

/*******
 * API *
 *******/

void PluginTaichi::InitApiCalls()
{
	DECLARE_PLUGIN_BUNNY_API_CALL("setFrequency(value)", PluginTaichi, Api_SetFrequency);
	DECLARE_PLUGIN_BUNNY_API_CALL("getFrequency()", PluginTaichi, Api_GetFrequency);
	DECLARE_PLUGIN_BUNNY_API_CALL("setTaichiOnClick(value)", PluginTaichi, Api_SetTaichiOnClick);
	DECLARE_PLUGIN_BUNNY_API_CALL("getTaichiOnClick()", PluginTaichi, Api_GetTaichiOnClick);
}

PLUGIN_BUNNY_API_CALL(PluginTaichi::Api_SetFrequency)
{
	Q_UNUSED(account);

	bunny->SetPluginSetting(GetName(), "frequency", QVariant(hRequest.GetArg("value").toInt()));
	SendTaichiFrequency(bunny);
	return new ApiManager::ApiOk(QString("Plugin configuration updated."));
}

PLUGIN_BUNNY_API_CALL(PluginTaichi::Api_GetFrequency)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);

	return new ApiManager::ApiString(QString::number(bunny->GetPluginSetting(GetName(), "frequency", 0).toInt()));
}

PLUGIN_BUNNY_API_CALL(PluginTaichi::Api_SetTaichiOnClick)
{
	Q_UNUSED(account);

	bunny->SetPluginSetting(GetName(), "taichiOnClick", QVariant(hRequest.GetArg("value") == "true" ? true : false));
	return new ApiManager::ApiOk(QString("Plugin configuration updated."));
}

PLUGIN_BUNNY_API_CALL(PluginTaichi::Api_GetTaichiOnClick)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);

	return new ApiManager::ApiString(bunny->GetPluginSetting(GetName(), "taichiOnClick", false).toBool() ? "true" : "false");
}
