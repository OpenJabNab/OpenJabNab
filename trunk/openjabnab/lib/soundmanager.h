#ifndef _SOUNDMANAGER_H_
#define _SOUNDMANAGER_H_

#include <QMap>
#include <QVector>
#include <QEventLoop>
#include <QObject>
#include "global.h"

class OJN_EXPORT SoundManager : public QObject
{
	Q_OBJECT
public:
	bool createNewSound(QString, QString, QString);

private slots:
	void downloadDone();

private:
	QEventLoop loop;

};

#endif
