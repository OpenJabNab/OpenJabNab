#ifndef _PLUGINMUSIC_H_
#define _PLUGINMUSIC_H_

#include <QHttp>
#include <QMultiMap>
#include <QTextStream>
#include <QThread>
#include "plugininterface.h"

class PluginMusic : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	PluginMusic();
	virtual ~PluginMusic() {}
	virtual bool Init();
	virtual bool OnRFID(Bunny *, QByteArray const&);
	virtual bool OnRFID(Ztamp *, Bunny *);

	bool OnClick(Bunny *, PluginInterface::ClickType);

	// API
	void InitApiCalls();
	PLUGIN_BUNNY_API_CALL(Api_AddRFID);
	PLUGIN_BUNNY_API_CALL(Api_RemoveRFID);
	PLUGIN_BUNNY_API_CALL(Api_getFilesList);

private:
	void getMusicList(Bunny *);
	QDir musicFolder;

};
#endif
