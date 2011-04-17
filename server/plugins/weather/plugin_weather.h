#ifndef _PLUGINWEATHER_H_
#define _PLUGINWEATHER_H_

#include <QHttp>
#include <QMultiMap>
#include <QTextStream>
#include <QThread>
#include "plugininterface.h"
	
class PluginWeather : public PluginInterface
{
	friend class PluginWeather_Worker;
	Q_OBJECT
	Q_INTERFACES(PluginInterface)
	
private slots:
	void analyseXml();
	void analyseDone(bool, Bunny*, QByteArray);

public:
	PluginWeather();
	virtual ~PluginWeather();
	bool OnClick(Bunny *, PluginInterface::ClickType);
	void OnCron(Bunny *, QVariant);
	void OnBunnyConnect(Bunny *);
	void OnBunnyDisconnect(Bunny *);
	void AfterBunnyUnregistered(Bunny *) {};

	// API
	void InitApiCalls();
	PLUGIN_BUNNY_API_CALL(Api_DefaultCity);
	PLUGIN_BUNNY_API_CALL(Api_AddWebcast);
	PLUGIN_BUNNY_API_CALL(Api_RemoveWebcast);
	PLUGIN_BUNNY_API_CALL(Api_ListWebcast);

private:
	void getWeatherPage(Bunny *, QString);
	QMultiMap<Bunny*, QPair<int, QString> > webcastList;
	QDir weatherFolder;

};

class PluginWeather_Worker : public QThread
{
	Q_OBJECT

signals:
	void done(bool, Bunny*, QByteArray);

public:
	PluginWeather_Worker(PluginWeather * , Bunny *, QByteArray);
	virtual ~PluginWeather_Worker() {}
	void run();

private:
	PluginWeather * plugin;
	Bunny * bunny;
	QByteArray buffer;
	QStringList weatherCodes;

};

#endif
