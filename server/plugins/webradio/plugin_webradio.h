#ifndef _PLUGINWEBRADIO_H_
#define _PLUGINWEBRADIO_H_

#include <QMap>
#include "plugininterface.h"
	
class PluginWebradio : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)
	
public:
	PluginWebradio();
	virtual ~PluginWebradio();
	virtual bool Init();
	void OnCron(Bunny *, QVariant);
	virtual bool OnRFID(Bunny *, QByteArray const&);
	virtual bool OnRFID(Ztamp *, Bunny *);
	
	bool OnClick(Bunny *, PluginInterface::ClickType);

	void OnBunnyConnect(Bunny *);
	void OnBunnyDisconnect(Bunny *);

	// API
	void InitApiCalls();
	PLUGIN_BUNNY_API_CALL(Api_AddRFID);
	PLUGIN_BUNNY_API_CALL(Api_RemoveRFID);
	PLUGIN_BUNNY_API_CALL(Api_AddPreset);
	PLUGIN_BUNNY_API_CALL(Api_RemovePreset);
	PLUGIN_BUNNY_API_CALL(Api_AddWebcast);
	PLUGIN_BUNNY_API_CALL(Api_RemoveWebcast);
	PLUGIN_BUNNY_API_CALL(Api_GetDefault);
	PLUGIN_BUNNY_API_CALL(Api_SetDefault);
	PLUGIN_BUNNY_API_CALL(Api_Play);
	PLUGIN_BUNNY_API_CALL(Api_PlayUrl);
	PLUGIN_BUNNY_API_CALL(Api_ListWebcast);
	PLUGIN_BUNNY_API_CALL(Api_ListPreset);
private:
	bool streamWebradio(Bunny *, QString);
	bool streamPresetWebradio(Bunny *, QString);
	QMap<QString, QVariant> presets;
};

#endif
