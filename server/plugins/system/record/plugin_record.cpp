#include <QDateTime>
#include <QStringList>
#include <memory>
#include "plugin_record.h"
#include "bunny.h"
#include "bunnymanager.h"
#include "log.h"
#include "settings.h"

Q_EXPORT_PLUGIN2(plugin_record, PluginRecord)

PluginRecord::PluginRecord():PluginInterface("record", "Manage Record requests", SystemPlugin)
{
	std::auto_ptr<QDir> dir(GetLocalHTTPFolder());
	if(dir.get())
	{
		recordFolder = *dir;
	}
}

bool PluginRecord::HttpRequestHandle(HTTPRequest & request)
{
	QString uri = request.GetURI();
	if (uri.startsWith("/vl/record.jsp"))
	{
		QString serialnumber = request.GetArg("sn");
		QString filename = recordFolder.absoluteFilePath("record_"+serialnumber+"_"+QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")+".wav");
		QFile wavFile( filename );
		if(wavFile.open(QFile::WriteOnly)) {
			wavFile.write(request.GetPostRaw());
			Bunny * b = BunnyManager::GetBunny(this, serialnumber.toAscii());
			b->SetGlobalSetting("LastRecord", filename);
		} else {
			LogError("Impossible to write record file");
		}
		return true;
	}
	return false;
}
