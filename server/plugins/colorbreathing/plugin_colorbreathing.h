#ifndef _PLUGINCOLORBREATHING_H_
#define _PLUGINCOLORBREATHING_H_

#include "plugininterface.h"
#include "httprequest.h"

class PluginColorbreathing : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	PluginColorbreathing();
	virtual ~PluginColorbreathing() {};
	void OnInitPacket(const Bunny *, AmbientPacket &, SleepPacket &);
	void InitApiCalls();

protected:
        PLUGIN_BUNNY_API_CALL(Api_GetColorList);
        PLUGIN_BUNNY_API_CALL(Api_SetColor);
        PLUGIN_BUNNY_API_CALL(Api_GetColor);

	QHash<QString, unsigned char> availableColors;
};

#endif
