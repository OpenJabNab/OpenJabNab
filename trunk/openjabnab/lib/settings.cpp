#include <QSettings>
#include <QCoreApplication>
#include <QDir>

#include "settings.h"
#include "log.h"

GlobalSettings::GlobalSettings()
{
	settings = new QSettings(QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("openjabnab.ini"), QSettings::IniFormat);
}

QVariant GlobalSettings::Get(QString const& key, QVariant const& defaultValue)
{
	if (!instance)
		instance = new GlobalSettings();

	if (instance->settings->contains(key))
		return instance->settings->value(key);
	else
	{
		Log::Warning(QString("Unknown setting : %1\n").arg(key));
		return defaultValue;
	}
}

GlobalSettings * GlobalSettings::instance = 0;
