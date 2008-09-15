#ifndef _OPENJABNAB_H_
#define _OPENJABNAB_H_

#include <QCoreApplication>
#include <QTcpServer>
#include "apimanager.h"
#include "pluginmanager.h"

class OpenJabNab : public QCoreApplication
{
	Q_OBJECT
	
public:
	OpenJabNab(int argc, char ** argv);
	~OpenJabNab();

private slots:
	void NewHTTPConnection();
	void NewXMPPConnection();
	void OnQuit();

private:
	QTcpServer * httpListener;
	QTcpServer * xmppListener;
	PluginManager * pluginManager;
	ApiManager * apiManager;
	bool httpApi;
	bool httpViolet;
};

#endif
