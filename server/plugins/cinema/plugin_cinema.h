#ifndef _PLUGINCINEMA_H_
#define _PLUGINCINEMA_H_

#include <QHttp>
#include <QMultiMap>
#include <QTextStream>
#include <QThread>
#include "plugininterface.h"
	
class PluginCinema : public PluginInterface
{
	friend class PluginCinema_Worker;
	Q_OBJECT
	Q_INTERFACES(PluginInterface)
	
private slots:
	void analyseXml();
	void analyseDone(bool, Bunny*, QByteArray);

public:
	PluginCinema();
	virtual ~PluginCinema();
	
	virtual bool OnClick(Bunny *, PluginInterface::ClickType);
	virtual void OnCron(Bunny * b, QVariant);
	virtual void OnBunnyConnect(Bunny *);
	virtual void OnBunnyDisconnect(Bunny *);

	// API
	virtual void InitApiCalls();
	PLUGIN_BUNNY_API_CALL(Api_AddWebcast);
	PLUGIN_BUNNY_API_CALL(Api_RemoveWebcast);
	PLUGIN_BUNNY_API_CALL(Api_GetWebcasts);

private:
	void getCinemaPage(Bunny *);
	QByteArray ceSoirMessage;
};

class PluginCinema_Worker : public QThread
{
	Q_OBJECT

signals:
	void done(bool, Bunny*, QByteArray);

public:
	PluginCinema_Worker(PluginCinema * , Bunny *, QByteArray);
	virtual ~PluginCinema_Worker() {}
	void run();

private:
	PluginCinema * plugin;
	Bunny * bunny;
	QByteArray buffer;

};

#endif
