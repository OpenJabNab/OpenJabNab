#ifndef _PLUGINJokes_H_
#define _PLUGINJokes_H_

#include <QHttp>
#include <QMultiMap>
#include <QTextStream>
#include <QThread>
#include "plugininterface.h"

class PluginJokes : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

private slots:
	void analyseXml();
	void analyseDone(bool, Bunny*, QByteArray);

public:
	PluginJokes();
	virtual ~PluginJokes();

	virtual bool Init();

	virtual bool OnClick(Bunny *, PluginInterface::ClickType);
	virtual void OnCron(Bunny * b, QVariant);
	virtual void OnBunnyConnect(Bunny *);
	virtual void OnBunnyDisconnect(Bunny *);

	// API
	virtual void InitApiCalls();
	PLUGIN_BUNNY_API_CALL(Api_SetFrequency);
	PLUGIN_BUNNY_API_CALL(Api_GetFrequency);
	PLUGIN_BUNNY_API_CALL(Api_SetLang);
	PLUGIN_BUNNY_API_CALL(Api_GetAvailableLang);
	PLUGIN_BUNNY_API_CALL(Api_GetCurrentLang);

protected:
        void createCron(Bunny *);
        int GetRandomizedFrequency(unsigned int);

private:
	void getJokes(Bunny *);
	QByteArray jokesMessage;
	QStringList availableLang;
};

#endif
