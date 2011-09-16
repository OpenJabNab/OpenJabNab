#ifndef _PLUGINTV_H_
#define _PLUGINTV_H_

#include <QHttp>
#include <QMultiMap>
#include <QTextStream>
#include <QThread>
#include "plugininterface.h"
	
class PluginTV : public PluginInterface
{
	friend class PluginTV_Worker;
	Q_OBJECT
	Q_INTERFACES(PluginInterface)
	
private slots:
	void analyseXml();
	void analyseDone(bool, Bunny*, QByteArray);

public:
	PluginTV();
	virtual ~PluginTV();
	
	virtual bool Init();
	
	virtual bool OnClick(Bunny *, PluginInterface::ClickType);
	virtual void OnCron(Bunny * b, QVariant);
	virtual void OnBunnyConnect(Bunny *);
	virtual void OnBunnyDisconnect(Bunny *);

	// API
	virtual void InitApiCalls();
	PLUGIN_BUNNY_API_CALL(Api_AddWebcast);
	PLUGIN_BUNNY_API_CALL(Api_RemoveWebcast);
	PLUGIN_BUNNY_API_CALL(Api_ListWebcast);
	PLUGIN_BUNNY_API_CALL(Api_SetChannel);
	PLUGIN_BUNNY_API_CALL(Api_ListChannel);

private:
	void getTVPage(Bunny *);
	QByteArray ceSoirMessage;
};

class PluginTV_Worker : public QThread
{
	Q_OBJECT

signals:
	void done(bool, Bunny*, QByteArray);

public:
	PluginTV_Worker(PluginTV * , Bunny *, QByteArray);
	virtual ~PluginTV_Worker() {}
	void run();

private:
	PluginTV * plugin;
	Bunny * bunny;
	QByteArray buffer;

};

#endif
