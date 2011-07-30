#include <QCryptographicHash>
#include <QDataStream>
#include <QEventLoop>
#include <QFile>
#include <QHttp>
#include <QHttpRequestHeader>
#include <QObject>
#include <QPluginLoader>
#include <QStringList>
#include <QUrl>
#include "log.h"
#include "settings.h"
#include "ttsmanager.h"
#include <cstdlib>

TTSManager::TTSManager()
{
		// Load all tts
		ttsDir = QCoreApplication::applicationDirPath();
		if (!ttsDir.cd("tts"))
		{
			LogError("Unable to open tts directory !\n");
			exit(-1);
		}
}

TTSManager & TTSManager::Instance()
{
  static TTSManager p;
  return p;
}

void TTSManager::UnloadTTSs()
{
        foreach(TTSInterface * p, listOfTTSs)
			delete p;

        foreach(QPluginLoader * l, listOfTTSsLoader.values())
        {
                l->unload();
                delete l;
        }
}

void TTSManager::LoadTTSs()
{
        LogInfo(QString("Finding tts in : %1").arg(ttsDir.path()));
		QStringList filters;
		filters << "*.so";
		ttsDir.setNameFilters(filters);
		foreach (QFileInfo file, ttsDir.entryInfoList(QDir::Files))
			LoadTTS(file.fileName().toAscii());
}

bool TTSManager::LoadTTS(QString const& fileName)
{
        if(listOfTTSsByFileName.contains(fileName))
        {
                LogError(QString("TTS '%1' already loaded !").arg(fileName));
                return false;
        }

        QString file = ttsDir.absoluteFilePath(fileName);
        if (!QLibrary::isLibrary(file))
                return false;

        QString status = QString("Loading %1 : ").arg(fileName);

        QPluginLoader * loader = new QPluginLoader(file);
        QObject * p = loader->instance();
        TTSInterface * tts = qobject_cast<TTSInterface *>(p);
        if (tts)
        {
                if(tts->Init() == false)
                {
                        delete tts;
                        loader->unload();
                        delete loader;

                        status.append(QString("%1 OK, Initialisation failed").arg(tts->GetName()));
                        LogInfo(status);
                        return false;
                }

                listOfTTSs.append(tts);
                listOfTTSsFileName.insert(tts, fileName);
                listOfTTSsLoader.insert(tts, loader);
                listOfTTSsByName.insert(tts->GetName(), tts);
                listOfTTSsByFileName.insert(fileName, tts);

                status.append(QString("%1 OK, Enable : %2").arg(tts->GetName(),tts->GetEnable() ? "Yes" : "No"));
                LogInfo(status);
                return true;
        }
        status.append("Failed, ").append(loader->errorString());
        LogInfo(status);
        return false;
}

bool TTSManager::UnloadTTS(QString const& name)
{
        if(listOfTTSsByName.contains(name))
        {
                TTSInterface * p = listOfTTSsByName.value(name);
                QString fileName = listOfTTSsFileName.value(p);
                QPluginLoader * loader = listOfTTSsLoader.value(p);
                listOfTTSsByFileName.remove(fileName);
                listOfTTSsFileName.remove(p);
                listOfTTSsLoader.remove(p);
                listOfTTSsByName.remove(name);
                listOfTTSs.removeAll(p);
                delete p;
                loader->unload();
                delete loader;
                LogInfo(QString("TTS %1 unloaded.").arg(name));
                return true;
        }
        LogInfo(QString("Can't unload tts %1").arg(name));
        return false;
}

bool TTSManager::ReloadTTS(QString const& name)
{
        if(listOfTTSsByName.contains(name))
        {
                TTSInterface * p = listOfTTSsByName.value(name);
                QString file = listOfTTSsFileName.value(p);
                return (UnloadTTS(name) && LoadTTS(file));
        }
        return false;
}

// Creatte TTS Song in /broadcast/tts/<name>/<voice>/[md5].mp3
QByteArray TTSManager::CreateNewSound(QString text, QString voice, QString name, bool forceOverwrite)
{
	TTSInterface * tts = Instance().GetTTSByName(name);
	return tts->CreateNewSound(text, voice, forceOverwrite);
}

QByteArray TTSManager::CreateNewSound(QString text, QString voice, bool forceOverwrite)
{
	TTSInterface * tts = Instance().GetTTSByName(GlobalSettings::Get("Config/TTS", "acapela").toString());
	return tts->CreateNewSound(text, voice, forceOverwrite);
}

