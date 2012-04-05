#include "plugin_TEMPLATELOWER.h"

Q_EXPORT_PLUGIN2(plugin_TEMPLATELOWER, TEMPLATECLASS)

TEMPLATECLASS::TEMPLATECLASS():PluginInterface("TEMPLATELOWER", "TEMPLATELOWER plugin")
{
}

TEMPLATECLASS::~TEMPLATECLASS() {}

void TEMPLATECLASS::OnBunnyConnect(Bunny *)
{
}

void TEMPLATECLASS::OnBunnyDisconnect(Bunny *)
{
}

void TEMPLATECLASS::OnInitPacket(const Bunny *, AmbientPacket &, SleepPacket &)
{
}
