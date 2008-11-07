#ifndef _CRON_H_
#define _CRON_H_

#include <QLinkedList>
#include <QObject>
#include <QVariant>
#include "global.h"

class PluginInterface;

typedef struct {
	PluginInterface * plugin;
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
	static unsigned int Register(PluginInterface *, unsigned int interval, unsigned int offsetH, unsigned int offsetM, QVariant data = QVariant(), const char * callback = 0);
	static unsigned int RegisterDaily(PluginInterface * p, QTime const& time, QVariant data, const char * callback);
	static unsigned int RegisterWeekly(PluginInterface * p, Qt::DayOfWeek day, QTime const& time, QVariant data, const char * callback);
	static void Unregister(PluginInterface *, unsigned int id);
	static void UnregisterAll(PluginInterface *);
	void AddCron(CronElement const&);

private slots:
	void OnTimer();
	
private:
	Cron();
	virtual ~Cron() {};
	static Cron& Instance();
	unsigned int lastGivenID;
	QLinkedList<CronElement> CronElements;
};

#endif
