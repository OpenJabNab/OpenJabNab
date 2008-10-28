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
	bool OnClick(Bunny *, PluginInterface::ClickType);
	void OnCron(QVariant);
	ApiManager::ApiAnswer * ProcessBunnyApiCall(Bunny *, Account const&, QString const&, HTTPRequest const&);
	void OnBunnyConnect(Bunny *);
	void OnBunnyDisconnect(Bunny *);
	void AfterBunnyUnregistered(Bunny *) {};

private:
	void getTVPage(Bunny *);
	QMultiMap<Bunny*, QPair<int, QString> > webcastList;
	QDir tvFolder;

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
