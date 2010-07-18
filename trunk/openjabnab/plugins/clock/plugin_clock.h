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
	void OnCron(Bunny*, QVariant);
	void OnBunnyConnect(Bunny *);
	void OnBunnyDisconnect(Bunny *);

private:
	QDir clockFolder;
	QList<Bunny*> bunnyList;
};

#endif
