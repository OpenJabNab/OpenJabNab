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
	
	bool OnClick(Bunny *, PluginInterface::ClickType);

private:
	void getMusicList(Bunny *);
	QDir musicFolder;

};
#endif
