#ifndef _PLUGINSLEEP_H_
#define _PLUGINSLEEP_H_

#include <QList>
#include <QMap>
#include <QPair>
#include <QStringList>
#include <QTime>
#include "plugininterface.h"
#include "httprequest.h"
	
typedef struct {
	QTime listOfTimes[7];
	int listOfCrons[7];
} Week;

class PluginSleep : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	PluginSleep();
	virtual ~PluginSleep();
	void OnCronSleep();
	void OnCronWakeUp();
	void OnBunnyConnect(Bunny *);
	void OnBunnyDisconnect(Bunny *);

//	ApiManager::ApiAnswer * ProcessBunnyApiCall(Bunny *, Account const&, QString const&, HTTPRequest const&);

	void InitApiCalls();
	PLUGIN_BUNNY_API_CALL(Api_GoodNight);
	PLUGIN_BUNNY_API_CALL(Api_Hello);
	PLUGIN_BUNNY_API_CALL(Api_Sleep);
	PLUGIN_BUNNY_API_CALL(Api_WakeUp);
private:

	void RegisterCrons(Bunny *);
	QList< int > listOfCrons;
	QMap<Bunny *, Week *> listOfSleep;
	QMap<Bunny *, Week *> listOfWakeUp;
};

#endif
