#ifndef _PLUGINWEATHER_H_
#define _PLUGINWEATHER_H_

#include <QUrl>
#include <QNetworkAccessManager>
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
	void analyseXml(QNetworkReply*);
	void analyseDone(bool, Bunny*, QByteArray);

public:
	PluginWeather();
	virtual ~PluginWeather();
	bool OnClick(Bunny *, PluginInterface::ClickType);
	bool OnRFID(Bunny * b, QByteArray const& tag);
	void OnCron(Bunny *, QVariant);
	void OnBunnyConnect(Bunny *);
	void OnBunnyDisconnect(Bunny *);
	void AfterBunnyUnregistered(Bunny *) {};

	// API
	void InitApiCalls();
	PLUGIN_BUNNY_API_CALL(Api_setDefaultCity);
	PLUGIN_BUNNY_API_CALL(Api_AddWebcast);
	PLUGIN_BUNNY_API_CALL(Api_RemoveWebcast);
	PLUGIN_BUNNY_API_CALL(Api_ListWebcast);
	PLUGIN_BUNNY_API_CALL(Api_addCity);
	PLUGIN_BUNNY_API_CALL(Api_removeCity);
	PLUGIN_BUNNY_API_CALL(Api_getCitiesList);
	PLUGIN_BUNNY_API_CALL(Api_getDefaultCity);
	PLUGIN_BUNNY_API_CALL(Api_AddRFID);
	PLUGIN_BUNNY_API_CALL(Api_RemoveRFID);
	PLUGIN_BUNNY_API_CALL(Api_getLang);
	PLUGIN_BUNNY_API_CALL(Api_setLang);
	PLUGIN_BUNNY_API_CALL(Api_getPrevitoken);
	PLUGIN_BUNNY_API_CALL(Api_setPrevitoken);

private:
	void getWeatherPage(Bunny *, QString);
	QDir weatherFolder;

};

class PluginWeather_Worker : public QThread
{
	Q_OBJECT

signals:
	void done(bool, Bunny*, QByteArray);

public:
	PluginWeather_Worker(PluginWeather * , Bunny *, QString);
	virtual ~PluginWeather_Worker() {}
	void run();

private:
	PluginWeather * plugin;
	Bunny * bunny;
	QString buffer;

};

#endif
