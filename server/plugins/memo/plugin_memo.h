#ifndef _PLUGINMEMO_H_
#define _PLUGINMEMO_H_

#include <QMultiMap>
#include "plugininterface.h"
	
class PluginMemo : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	PluginMemo();
	virtual ~PluginMemo();

	void OnBunnyConnect(Bunny *);
	void OnBunnyDisconnect(Bunny *);
	void OnCron(Bunny *, QVariant);
	void AfterBunnyUnregistered(Bunny *) {};

	// API
	void InitApiCalls();
	PLUGIN_BUNNY_API_CALL(Api_AddWebcast);
	PLUGIN_BUNNY_API_CALL(Api_RemoveWebcast);
	PLUGIN_BUNNY_API_CALL(Api_ListWebcast);
	PLUGIN_BUNNY_API_CALL(Api_AddDailyWebcast);
	PLUGIN_BUNNY_API_CALL(Api_RemoveDailyWebcast);
	PLUGIN_BUNNY_API_CALL(Api_ListDailyWebcast);

private:
	QDir memoFolder;
};

#endif
