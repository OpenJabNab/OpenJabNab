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
	static void Init();
	static void Close();

	// Without default value
	static QVariant Get(QString const& key);
	static QString GetString(QString const& key);
	static int GetInt(QString const& key);

	// With default value
	static QVariant Get(QString const& key, QVariant const& defaultValue);
	static QString GetString(QString const& key, QString const& defaultValue);
	static int GetInt(QString const& key, int defaultValue);
	
	static bool HasKey(QString const& key);
	
private:
	GlobalSettings();
	static GlobalSettings * instance;
	QSettings * settings;
};

inline void GlobalSettings::Init()
{
	if (!instance)
		instance = new GlobalSettings();
}

inline void GlobalSettings::Close()
{
	if (instance)
		delete instance;
}

inline QString GlobalSettings::GetString(QString const& key)
{
	return Get(key).toString();
}

inline int GlobalSettings::GetInt(QString const& key)
{
	return Get(key).toInt();
}

inline QString GlobalSettings::GetString(QString const& key, QString const& defaultValue)
{
	return Get(key, defaultValue).toString();
}

inline int GlobalSettings::GetInt(QString const& key, int defaultValue)
{
	return Get(key, defaultValue).toInt();
}
#endif
