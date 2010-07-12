#ifndef _PLUGINSLEEP_H_
#define _PLUGINSLEEP_H_

#include <QList>
#include <QMap>
#include <QPair>
#include <QStringList>
#include <QTime>
#include "plugininterface.h"
#include "httprequest.h"
	
class PluginSleep : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public slots:
	void OnCronSleep(Bunny *, QVariant);
	void OnCronWakeUp(Bunny *, QVariant);
	
public:
	PluginSleep();
	virtual ~PluginSleep();

	void OnBunnyConnect(Bunny *);
	void OnBunnyDisconnect(Bunny *);

	void InitApiCalls();

	PLUGIN_BUNNY_API_CALL(Api_GoodNight);
	PLUGIN_BUNNY_API_CALL(Api_Hello);
	PLUGIN_BUNNY_API_CALL(Api_Setup);

private:
	void RegisterCrons(Bunny *);
	void CleanCrons(Bunny *);
};

#endif
