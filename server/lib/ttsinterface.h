#ifndef _PLUGININTERFACE_H_
#define _PLUGININTERFACE_H_

#include <QByteArray>
#include <QCoreApplication>
#include <QDir>
#include <QSettings>
#include <QString>
#include <QtPlugin>
#include "log.h"
#include "settings.h"

class Account; 

class TTSInterface : public QObject
{
	friend class TTSManager;
public:

	TTSInterface(QString name, QString visualName = QString());
	virtual ~TTSInterface();
	
	// Called to init tts, return false if something is wrong
	virtual bool Init() { return true; };

	virtual QByteArray CreateNewSound(QString, QString, bool) { return ""; }

	// Settings
	QVariant GetSettings(QString const& key, QVariant const& defaultValue = QVariant()) const;
	void SetSettings(QString const& key, QVariant const& value);

	// Plugin's name
	QString const& GetName() const;
	QString const& GetVisualName() const;

	// Plugin enable/disable functions
	bool GetEnable() const;

	// Plugin type
	int GetType() const;

protected:
	void SetEnable(bool);
	QStringList voiceList;
	QDir ttsFolder;
	QString ttsHTTPUrl;

	QSettings * settings;

private:
	QString ttsName;
	QString ttsVisualName;
	bool ttsEnable;
};

#include "ttsinterface_inline.h"

Q_DECLARE_INTERFACE(TTSInterface,"org.toms.openjabnab.TTSInterface/1.0")

#endif
