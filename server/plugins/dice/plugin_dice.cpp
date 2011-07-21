#include <stdlib.h>
#include <time.h>
#include "bunny.h"
#include "httprequest.h"
#include "log.h"
#include "messagepacket.h"
#include "plugin_dice.h"

Q_EXPORT_PLUGIN2(plugin_dice, PluginDice)

PluginDice::PluginDice():PluginInterface("dice", "Dice roll",BunnyZtampPlugin)
{
	// Initialize the randomizer
	srand(time(NULL));
}

bool PluginDice::OnClick(Bunny * b, PluginInterface::ClickType type)
{
	if (type == PluginInterface::SingleClick)
	{
		// Language
		QByteArray Language = b->GetPluginSetting("dice", "PluginConfiguration/Language", "fr").toByteArray();
		// Get a random value and create ID
		quint8 value = rand() % 6 + 1;
		LogDebug(QString(" -- Language : %1 -- Roll dice : %2").arg(Language, QString::number(value)));
		// Send packet to bunny with mp3 to be played
		b->SendPacket(MessagePacket("MU "+GetBroadcastHTTPPath(Language + "/get.mp3")+"\nMW\nMU "+GetBroadcastHTTPPath(Language + "/" + QString::number(value) + ".mp3")+"\nMW\n"));
	}
	return true;
}

PluginDice::~PluginDice()
{
}
