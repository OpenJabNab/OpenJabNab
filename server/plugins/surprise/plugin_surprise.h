#ifndef _PLUGINSURPRISE_H_
#define _PLUGINSURPRISE_H_

#include "plugininterface.h"
	
class PluginSurprise : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	PluginSurprise();
	virtual ~PluginSurprise();

	void OnBunnyConnect(Bunny *);
	void OnBunnyDisconnect(Bunny *);
	virtual void OnCron(Bunny *, QVariant);
	
	void InitApiCalls();
	
protected:
	void createCron(Bunny * b);
	PLUGIN_BUNNY_API_CALL(Api_GetFolderList);
	PLUGIN_BUNNY_API_CALL(Api_SetFolder);

	QStringList availableSurprises;
};

#endif
