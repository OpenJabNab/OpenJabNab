#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <iostream>
#include "plugin_dump.h"
#include "log.h"
#include "packet.h"
#include "settings.h"

Q_EXPORT_PLUGIN2(plugin_dump, PluginDump)

PluginDump::PluginDump():PluginInterface("dump", "Dump", SystemPlugin)
{
	// Open dump file
	QFile * dumpFile = new QFile(QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("dump.log"), this);
	if(!dumpFile->open(QIODevice::Append))
	{
		Log::Error(QString("Error opening file : %1").arg(dumpFile->fileName()));
		return;
	}
	dumpStream.setDevice(dumpFile);
	dumpStream << QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss") << " -- OpenJabNab Start --" << endl;
}

void PluginDump::HttpRequestBefore(HTTPRequest const& request)
{
	dumpStream << QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss") << " - HTTP Request - " << request.GetRawURI() << endl;
}

void PluginDump::XmppBunnyMessage(Bunny *, QByteArray const& msg)
{
	dumpStream << QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss") << " - XMPP Bunny - " << msg << endl;
}

void PluginDump::XmppVioletMessage(Bunny *, QByteArray const& msg)
{
	dumpStream << QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss") << " - XMPP Violet - " << msg << endl;
}

bool PluginDump::XmppVioletPacketMessage(Bunny *, Packet const& p)
{
	dumpStream << QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss") << " - XMPP Violet Packet - " << p.GetPrintableData() << endl;
	return false;
}

PluginDump::~PluginDump()
{
	dumpStream << QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss") << " -- OpenJabNab End --" << endl;
	QIODevice * d = dumpStream.device();
	if (d)
		d->close();
}
