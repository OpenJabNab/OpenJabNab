#ifndef _TTSMANAGER_H_
#define _TTSMANAGER_H_

#include <QEventLoop>
#include <QObject>
#include "global.h"

class OJN_EXPORT TTSManager : public QObject
{
	Q_OBJECT
public:
	bool createNewSound(QString, QString, QString, bool);
	bool createNewSound(QString text, QString voice, QString file) { return createNewSound(text, voice, file, false); };

private slots:
	void downloadDone();

private:
	QEventLoop loop;

};

#endif
