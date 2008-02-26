#include <QDateTime>
#include "bunny.h"
#include "httprequest.h"
#include "log.h"
#include "messagepacket.h"
#include "plugin_dice.h"

Q_EXPORT_PLUGIN2(plugin_dice, PluginDice)

PluginDice::PluginDice():PluginInterface("dice")
{
	// Initialize the randomizer
	srand(time(NULL));
	// Create a file for values
	QFile * diceFile = new QFile(QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("dice.log"), this);
	if(!diceFile->open(QIODevice::Append))
	{
		Log::Error(QString("Error opening file : %1").arg(diceFile->fileName()));
		return;
	}
	diceStream.setDevice(diceFile);
}

bool PluginDice::OnClick(Bunny * b, PluginInterface::ClickType type)
{
	if (type == PluginInterface::SingleClick)
	{
		// Language
		QByteArray Language = b->GetPluginSetting("dice", "PluginConfiguration/Language", "fr").toByteArray();
		// Get a random value and create ID
		quint8 value = rand() % 6 + 1;
		// Log dice value
		diceStream << QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss") << " -- Language : " << Language << " -- Roll dice : " << value << endl;
		// Send packet to bunny with mp3 to be played
		b->SendPacket(MessagePacket("MU broadcast/ojn_local/plugins/dice/"+Language+"/get.mp3\nMW\nMU broadcast/ojn_local/plugins/dice/"+Language+"/"+QByteArray::number(value)+".mp3\nMW\n"));
	}
	else
	{
		// Shutdown of the plugin on Dbl click
		// To be done when activation / deactivation is OK
	}
	return true;
}

PluginDice::~PluginDice()
{
	QIODevice * d = diceStream.device();
	if (d)
		d->close();
}
