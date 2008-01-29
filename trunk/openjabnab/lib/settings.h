#ifndef _GLOBALSETTINGS_H_
#define _GLOBALSETTINGS_H_

#include <QObject>
#include <QString>
#include <QSettings>
#include <QVariant>
#include "global.h"

class OJN_EXPORT GlobalSettings
{
public:
	static QVariant Get(QString const& key, QVariant const& defaultValue = QVariant());
	static QString GetString(QString const& key, QString const& defaultValue = QString()) { return Get(key, defaultValue).toString(); }
	static int GetInt(QString const& key, int defaultValue = 0) {return Get(key, defaultValue).toInt(); }
	static bool HasKey(QString const& key);
	
private:
	GlobalSettings();
	static GlobalSettings * instance;
	QSettings * settings;
};

#endif
