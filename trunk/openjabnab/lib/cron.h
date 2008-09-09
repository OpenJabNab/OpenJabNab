#ifndef _CRON_H_
#define _CRON_H_

#include "global.h"
#include <QObject>
#include <QVariant>
#include <QList>

class PluginInterface;

typedef struct {
	QObject * plugin;
	QVariant data;
	const char * callback;
	unsigned int id;
	unsigned int interval; // in min
	unsigned int next_run; // in seconds since 1970-01-01T00:00:00
} CronElement;

class OJN_EXPORT Cron : public QObject
{
	Q_OBJECT
	
public:
	static unsigned int Register(QObject *, unsigned int interval, unsigned int offsetH, unsigned int offsetM, QVariant data = QVariant(), const char * callback = 0);
	static void Unregister(QObject *, unsigned int id);
	static void UnregisterAll(QObject *);
	static QList<CronElement> GetByData(QObject *, QVariant);

private slots:
	void OnTimer();
	
private:
	Cron();
	static Cron& Instance();
	unsigned int lastGivenID;
	QList<CronElement> CronElements;
};

#endif
