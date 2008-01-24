#ifndef _OPENJABNAB_H_
#define _OPENJABNAB_H_

#include <QCoreApplication>
#include <QTcpServer>
#include <QVector>
#include "plugininterface.h"

class OpenJabNab : public QCoreApplication
{
	Q_OBJECT
	
public:
	OpenJabNab(int argc, char ** argv);
	~OpenJabNab();
	static QVector<PluginInterface *> const& GetPlugins() { return listOfPlugins;} ;

private slots:
    void newHTTPConnection();
    void newXMPPConnection();

private:
	QTcpServer * httpListener;
	QTcpServer * xmppListener;
	static QVector<PluginInterface *> listOfPlugins;
};

#endif
