#ifndef _TTSMANAGER_H_
#define _TTSMANAGER_H_

#include <QHash>
#include <QMap>
#include <QList>
#include "global.h"
#include "ttsinterface.h"

class TTSInterface;
class QPluginLoader;
class OJN_EXPORT TTSManager
{
public:
        static TTSManager & Instance();
	static void Init();
	static void Close();
	static QByteArray CreateNewSound(QString, QString, bool overwrite = false);
	static QByteArray CreateNewSound(QString, QString, QString, bool overwrite = false);
	TTSInterface * GetTTSByName(QString const& name) const;
	
protected:
	static QStringList voiceList;
	static QDir ttsFolder;
	static QString ttsHTTPUrl;
private:
	TTSManager();
        void LoadTTSs();
        void UnloadTTSs();
        bool LoadTTS(QString const&); 
        bool UnloadTTS(QString const&); 
        bool ReloadTTS(QString const&); 
        QDir ttsDir;
        QList<TTSInterface *> listOfTTSs;
        QMap<TTSInterface *, QString> listOfTTSsFileName;
        QMap<TTSInterface *, QPluginLoader *> listOfTTSsLoader;
        QHash<QString, TTSInterface *> listOfTTSsByName;
        QHash<QString, TTSInterface *> listOfTTSsByFileName;

};

inline void TTSManager::Init()
{
        Instance().LoadTTSs();
}

inline void TTSManager::Close()
{
        Instance().UnloadTTSs();
}

inline TTSInterface * TTSManager::GetTTSByName(QString const& name) const
{
	if(listOfTTSsByName.contains(name))
		return listOfTTSsByName.value(name);
	return listOfTTSsByName.value("acapela");
	
}


#endif
