#ifndef _PLUGINTV_H_
#define _PLUGINTV_H_

#include <QHttp>
#include <QTextStream>
#include "plugininterface.h"
	
class PluginTV : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)
	
private slots:
	void analyseXml();

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
	QMap <int, QStringList> webcastList;
	QDir tvFolder;

};

#endif
