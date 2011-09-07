#ifndef _PLUGINAIRQUALITY_H_
#define _PLUGINAIRQUALITY_H_

#include <QUrl>
#include <QNetworkAccessManager>
#include <QMultiMap>
#include <QTextStream>
#include <QThread>
#include "plugininterface.h"
	
class PluginAirquality : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

private slots:
	void analyseHtml(QNetworkReply*);
	void analyseDone(bool, Bunny*, QByteArray);

public:
	PluginAirquality();
	virtual ~PluginAirquality();
	bool OnClick(Bunny *, PluginInterface::ClickType);
	bool OnRFID(Bunny * b, QByteArray const& tag);
	void OnCron(Bunny *, QVariant);
	void OnBunnyConnect(Bunny *);
	void OnBunnyDisconnect(Bunny *);
	void AfterBunnyUnregistered(Bunny *) {};

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

private:
	void getAirPage(Bunny *, QString);
	QDir airFolder;
	QStringList cityList;

};

class PluginAir_Worker : public QThread
{
	Q_OBJECT

signals:
	void done(bool, Bunny*, QByteArray);

public:
	PluginAir_Worker(PluginAirquality * , Bunny *, QString, QString);
	virtual ~PluginAir_Worker() {}
	void run();

private:
	PluginAirquality * plugin;
	Bunny * bunny;
	QString city;
	QString buffer;
	QStringList qualities;
};
#endif
