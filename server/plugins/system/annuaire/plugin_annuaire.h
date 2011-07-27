#ifndef _PLUGINANNUAIRE_H_
#define _PLUGINANNUAIRE_H_

#include "plugininterface.h"

class PluginAnnuaire : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	PluginAnnuaire();
	virtual ~PluginAnnuaire();

	void OnBunnyConnect(Bunny *);

	void InitApiCalls();
	PLUGIN_API_CALL(Api_setURL);
	PLUGIN_API_CALL(Api_getURL);

};

#endif
