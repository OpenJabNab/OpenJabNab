#include <QDateTime>
#include <QCryptographicHash>
#include <QXmlStreamReader>
#include <QHttp>
#include <QMapIterator>
#include <QRegExp>
#include <QUrl>
#include <QTextDocument>
#include <QTextCodec>
#include <memory>
#include "bunny.h"
#include "bunnymanager.h"
#include "httprequest.h"
#include "log.h"
#include "cron.h"
#include "messagepacket.h"
#include "plugin_jokes.h"
#include "settings.h"
#include "ttsmanager.h"

Q_EXPORT_PLUGIN2(plugin_blague, PluginJokes)

// +/- 20% - 30min => rand(24,36)
#define RANDOMIZEDRATIO 20

PluginJokes::PluginJokes():PluginInterface("jokes", "Jokes",BunnyZtampPlugin) {}

PluginJokes::~PluginJokes() {}

bool PluginJokes::Init()
{
	QByteArray currentBlague = TTSManager::CreateNewSound("Allez, encore une petite blague", "Claire");
	if(currentBlague.isNull())
		return false;

	jokesMessage = "MU " + currentBlague + "\nPL 3\nMW\n";
	return true;
}

void PluginJokes::createCron(Bunny * b)
{
        // Check Frequency
        unsigned int frequency = b->GetPluginSetting(GetName(), "frequency", (uint)0).toUInt();
        if(!frequency)
        {
                // Stable. Warning Removed.
                LogDebug(QString("Bunny '%1' has invalid frequency '%2'").arg(b->GetID(), QString::number(frequency)));
                return;
        }

	if (frequency > 1)
	{
		// Register cron
		Cron::RegisterOneShot(this, GetRandomizedFrequency(frequency), b, QVariant(), NULL);
	}
}

int PluginJokes::GetRandomizedFrequency(unsigned int freq)
{
        // 250 => ~30min, 125 => ~1h, 50 => ~2h30
        unsigned int meanTimeInSec = (250/freq) * 30;

        int deviation = 0;

        if(RANDOMIZEDRATIO > 0 && RANDOMIZEDRATIO < 100)
        {
                unsigned int maxDeviation = (meanTimeInSec * 2 * RANDOMIZEDRATIO) / 100;
                if(maxDeviation > 0)
                {
                        deviation = qrand() % (maxDeviation);
                }
                deviation -= (maxDeviation/2);
        }

        return meanTimeInSec + deviation;
}

void PluginJokes::OnCron(Bunny * b, QVariant)
{
	getJokes(b);
        // Restart Timer
        createCron(b);
}

bool PluginJokes::OnClick(Bunny * b, PluginInterface::ClickType type)
{
	if (type == PluginInterface::SingleClick)
	{
		getJokes(b);
		return true;
	}
	return false;
}

void PluginJokes::getJokes(Bunny * b)
{
	QDir * dir = GetLocalHTTPFolder();
	QByteArray message = jokesMessage;
	QStringList list;
	QString lang = b->GetPluginSetting(GetName(), "lang", QString()).toString();
	qsrand(QDateTime::currentDateTime ().toTime_t ());
	if (dir)
	{
		dir->cd("lang/" + lang + "/sources");
		dir->setNameFilters(QStringList("*.txt"));
		QStringList list = dir->entryList(QDir::Files|QDir::NoDotAndDotDot);
		// Get a random file
		QString file = list.at(qrand()%list.count());
		QString filepath = dir->absoluteFilePath(file);
		// LogDebug("Using joke file : " + filepath);
		// Get pre message if exist
		QFile blagueFilePre (filepath + ".pre");
		if (blagueFilePre.exists())
		{
			blagueFilePre.open(QFile::ReadOnly);
			QTextStream preflux(&blagueFilePre);
			preflux.setCodec(QTextCodec::codecForName("UTF-8"));
			QString premessage = preflux.readLine();
			// LogDebug("Premessage found : " + premessage);
			QByteArray currentBlague = TTSManager::CreateNewSound(premessage, "Claire");
			message = "MU " + currentBlague + "\nPL 3\nMW\n";
			blagueFilePre.close();
		}

		int line_count=0;
		QFile blagueFile(filepath);
		if(blagueFile.open(QFile::ReadOnly))
		{
			QTextStream flux(&blagueFile);
			flux.setCodec(QTextCodec::codecForName("UTF-8"));
			// First count lines in file
			while( !flux.atEnd())
			{
				flux.readLine();
				line_count++;
			}
			// Random line
			qsrand(QDateTime::currentDateTime ().toTime_t ());
			int random = qrand() % line_count;
			// Seek to begin
			flux.seek(0);
			// Read again and stop on the chosen line
			int loop = 0;
			while( !flux.atEnd())
			{
				QString blagounette = flux.readLine();
				if (loop == random)
				{
					// LogDebug("Selected jokes : " + blagounette); 
					QRegExp sep("[. ][:][- ]+");
					QStringList splitted_blagounette = blagounette.split(sep);
					foreach (const QString &word, splitted_blagounette) {
						if (!word.trimmed().isEmpty())
						{
							QByteArray file = TTSManager::CreateNewSound(word.trimmed(), "Claire");
							message += "MU " + file + "\nPL 3\nMW\n";
						}
					}
					break;
				}
				loop++;
			}
			blagueFile.close();
			if(b->IsConnected())
			{
				QByteArray file = GetBroadcastHTTPPath("mp3/joke_sound.mp3");
				message += "MU "+file+"\nPL 3\nMW\n";
				b->SendPacket(MessagePacket(message));
			}
		}
	}
}

void PluginJokes::OnBunnyConnect(Bunny * b)
{
	createCron(b);
}

void PluginJokes::OnBunnyDisconnect(Bunny * b)
{
	Cron::UnregisterAllForBunny(this, b);
}

/*******
 * API *
 *******/

void PluginJokes::InitApiCalls()
{
        DECLARE_PLUGIN_BUNNY_API_CALL("setFrequency(value)", PluginJokes, Api_SetFrequency);
        DECLARE_PLUGIN_BUNNY_API_CALL("getFrequency()", PluginJokes, Api_GetFrequency);
        DECLARE_PLUGIN_BUNNY_API_CALL("setLang()", PluginJokes, Api_SetLang);
        DECLARE_PLUGIN_BUNNY_API_CALL("getAvailableLang()", PluginJokes, Api_GetAvailableLang);
        DECLARE_PLUGIN_BUNNY_API_CALL("getCurrentLang()", PluginJokes, Api_GetCurrentLang);
}

PLUGIN_BUNNY_API_CALL(PluginJokes::Api_SetFrequency)
{
        Q_UNUSED(account);

        bunny->SetPluginSetting(GetName(), "frequency", QVariant(hRequest.GetArg("value").toInt()));
        OnBunnyDisconnect(bunny);
        OnBunnyConnect(bunny);
        return new ApiManager::ApiOk(QString("Plugin configuration updated."));
}

PLUGIN_BUNNY_API_CALL(PluginJokes::Api_GetFrequency)
{
        Q_UNUSED(account);
        Q_UNUSED(hRequest);

        return new ApiManager::ApiOk(QString::number(bunny->GetPluginSetting(GetName(), "frequency", (uint)0).toInt()));
}

PLUGIN_BUNNY_API_CALL(PluginJokes::Api_SetLang)
{
        Q_UNUSED(account);

        QString lang = hRequest.GetArg("value");
        if(availableLang.contains(lang))
        {
                // Save new config
                bunny->SetPluginSetting(GetName(), "lang", lang);

                return new ApiManager::ApiOk(QString("Folder changed to '%1'").arg(lang));
        }
        return new ApiManager::ApiError(QString("Unknown '%1' lang").arg(lang));
}

PLUGIN_BUNNY_API_CALL(PluginJokes::Api_GetCurrentLang)
{
        Q_UNUSED(account);
        Q_UNUSED(hRequest);

        return new ApiManager::ApiOk(bunny->GetPluginSetting(GetName(), "lang", QString()).toString());
}

PLUGIN_BUNNY_API_CALL(PluginJokes::Api_GetAvailableLang)
{
        Q_UNUSED(account);
	Q_UNUSED(bunny);
        Q_UNUSED(hRequest);

	QDir * dir = GetLocalHTTPFolder();
	availableLang.clear();
	if (dir)
	{
		dir->cd("lang");
		QStringList havailableLang = dir->entryList(QDir::AllDirs|QDir::NoDotAndDotDot);
		foreach (const QString &tDirectory, havailableLang)
		{
			availableLang.append(tDirectory);
		}
	}

	return new ApiManager::ApiList(availableLang);
}
