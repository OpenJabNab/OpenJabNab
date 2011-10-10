#ifndef _PLUGINANNUAIRE_H_
#define _PLUGINANNUAIRE_H_

#include "plugininterface.h"
#include "bunny.h"

typedef struct {
	QString name;
	QByteArray ID;
	QString server;
} BunnyInfos;

class PluginAnnuaire : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	PluginAnnuaire();
	virtual ~PluginAnnuaire();

	void OnBunnyConnect(Bunny *);
	QList<BunnyInfos> SearchBunnyByName(QString name);
	QList<BunnyInfos> SearchBunnyByMac(QByteArray ID);

	void InitApiCalls();
	PLUGIN_API_CALL(Api_setURL);
	PLUGIN_API_CALL(Api_getURL);
	PLUGIN_API_CALL(Api_SearchBunnyByName);
	PLUGIN_API_CALL(Api_SearchBunnyByMac);
	PLUGIN_API_CALL(Api_VerifyMacToken);

};

#endif
