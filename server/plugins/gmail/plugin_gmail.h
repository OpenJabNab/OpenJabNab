#ifndef _PLUGINGMAIL_H_
#define _PLUGINGMAIL_H_

#include <QHttp>
#include <QMultiMap>
#include <QTextStream>
#include <QThread>
#include <QTcpSocket>
#include "plugininterface.h"
#include <QXmlStreamReader>

class PluginGmail : public PluginInterface
{
Q_OBJECT
Q_INTERFACES(PluginInterface)

public:
PluginGmail();
virtual ~PluginGmail();
void OnCron(Bunny*, QVariant);
void OnBunnyConnect(Bunny *);
void OnBunnyDisconnect(Bunny *);
void InitApiCalls();
PLUGIN_BUNNY_API_CALL(Api_SetConfig);
PLUGIN_BUNNY_API_CALL(Api_GetConfig);

private:
QMap<Bunny*, QString> bunnyList;
QTcpSocket *socket;
    int emailsCount;
    int connectionId;
    QXmlStreamReader xml;
    QHttp http;
    QString currentTag;


private slots:
    void readData(const QHttpResponseHeader &);



};

#endif

