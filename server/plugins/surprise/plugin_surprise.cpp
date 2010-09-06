#include "plugin_surprise.h"

Q_EXPORT_PLUGIN2(plugin_surprise, PluginSurprise)

PluginSurprise::PluginSurprise():PluginInterface("template", "template plugin")
{
}

PluginSurprise::~PluginSurprise() {}

void PluginSurprise::OnBunnyConnect(Bunny *)
{
}

void PluginSurprise::OnBunnyDisconnect(Bunny *)
{
}

void PluginSurprise::OnInitPacket(const Bunny *, AmbientPacket &, SleepPacket &)
{
}
