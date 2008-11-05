#include <QRegExp>
#include "plugin_sleep.h"
#include "bunny.h"
#include "log.h"
#include "messagepacket.h"
#include "packet.h"
#include "settings.h"
#include "sleeppacket.h"

Q_EXPORT_PLUGIN2(plugin_sleep, PluginSleep)

PluginSleep::PluginSleep():PluginInterface("sleep", "Advanced sleep and wake up")
{
}

PluginSleep::~PluginSleep() {}

ApiManager::ApiAnswer * PluginSleep::ProcessBunnyApiCall(Bunny * b, Account const&, QString const& funcName, HTTPRequest const& r)
{
	if(funcName == "goodNight")
	{
		b->SendPacket(SleepPacket(SleepPacket::Sleep));
		return new ApiManager::ApiOk(QString("Bunny is going to sleep."));
	}
	else if(funcName == "hello")
	{
		b->SendPacket(SleepPacket(SleepPacket::Wake_Up));
		return new ApiManager::ApiOk(QString("Bunny is waking up."));
	}
	else
		return new ApiManager::ApiError(QString("Bad function name for plugin " + GetVisualName()));
}
