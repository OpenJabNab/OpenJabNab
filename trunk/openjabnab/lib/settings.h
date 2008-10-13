#ifndef _GLOBALSETTINGS_H_
#define _GLOBALSETTINGS_H_

#include <QObject>
#include <QSettings>
#include <QString>
#include <QVariant>
#include "global.h"

class OJN_EXPORT GlobalSettings
{
public:
	~GlobalSettings();
	static inline void Init() { if (!instance) instance = new GlobalSettings(); }
	static inline void Close() { if (instance) delete instance; }

	// Without default value
	static QVariant Get(QString const& key);
	static QString GetString(QString const& key) { return Get(key).toString(); }
	static int GetInt(QString const& key) {return Get(key).toInt(); }

	// With default value
	static QVariant Get(QString const& key, QVariant const& defaultValue);
	static QString GetString(QString const& key, QString const& defaultValue) { return Get(key, defaultValue).toString(); }
	static int GetInt(QString const& key, int defaultValue) {return Get(key, defaultValue).toInt(); }
	
	static bool HasKey(QString const& key);
	
private:
	GlobalSettings();
	static GlobalSettings * instance;
	QSettings * settings;
};

#endif
