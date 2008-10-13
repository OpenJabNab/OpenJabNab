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
	virtual ~OpenJabNab();

private slots:
	void NewHTTPConnection();
	void NewXMPPConnection();

private:
	QTcpServer * httpListener;
	QTcpServer * xmppListener;
	bool httpApi;
	bool httpViolet;
};

#endif
