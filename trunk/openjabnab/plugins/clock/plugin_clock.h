#ifndef _PLUGINCLOCK_H_
#define _PLUGINCLOCK_H_

#include <QHttp>
#include <QMultiMap>
#include <QTextStream>
#include <QThread>
#include "plugininterface.h"
	
class PluginClock : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)
	
public:
	PluginClock();
	virtual ~PluginClock();
	virtual bool Init();
	void OnCron(QVariant);
	void OnBunnyConnect(Bunny *);
	void OnBunnyDisconnect(Bunny *);

private:
	QMultiMap<Bunny*, int> webcastList;
	QDir clockFolder;

};

#endif
