#ifndef _PLUGINRATP_H_
#define _PLUGINRATP_H_

#include <QUrl>
#include <QNetworkAccessManager>
#include <QMultiMap>
#include <QTextStream>
#include <QThread>
#include "plugininterface.h"
	
	
class PluginRatp : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

private slots:
	void analyseXml(QNetworkReply* networkReply);
	void analyseDone(bool, Bunny*, QByteArray);
	
public:
	PluginRatp();
	virtual ~PluginRatp();
	
	bool OnClick(Bunny *, PluginInterface::ClickType);
	void OnCron(Bunny *, QVariant);
	bool OnRFID(Bunny * b, QByteArray const& tag);
	void OnBunnyConnect(Bunny *);
	void OnBunnyDisconnect(Bunny *);
	void OnInitPacket(const Bunny *, AmbientPacket &, SleepPacket &);

	// API
	void InitApiCalls();
	PLUGIN_BUNNY_API_CALL(Api_setDefaultArret);
	PLUGIN_BUNNY_API_CALL(Api_addArret);
	PLUGIN_BUNNY_API_CALL(Api_removeArret);
	PLUGIN_BUNNY_API_CALL(Api_getListArret);
	PLUGIN_BUNNY_API_CALL(Api_getArretDefaut);
	PLUGIN_BUNNY_API_CALL(Api_AddWebcast);
	PLUGIN_BUNNY_API_CALL(Api_RemoveWebcast);
	PLUGIN_BUNNY_API_CALL(Api_ListWebcast);
	PLUGIN_BUNNY_API_CALL(Api_AddRFID);
	PLUGIN_BUNNY_API_CALL(Api_RemoveRFID);
	
private:
	void getPageHoraire(Bunny * b, QString reseau, QString ville, QString arret, QString direction);

};

class PluginRatp_Worker : public QThread
{
	Q_OBJECT

signals:
	void done(bool, Bunny*, QByteArray);

public:
	PluginRatp_Worker(PluginRatp * , Bunny *, QString);
	virtual ~PluginRatp_Worker() {}
	void run();

private:
	PluginRatp * plugin;
	Bunny * bunny;
	QString buffer;

};

#endif
