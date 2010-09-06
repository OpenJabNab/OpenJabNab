#ifndef _CRON_H_
#define _CRON_H_

#include <QLinkedList>
#include <QObject>
#include <QVariant>
#include "global.h"

class PluginInterface;
class Bunny;

typedef struct {
	PluginInterface * plugin;
	Bunny * bunny;
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
	// Will fire at hh:mm and every interval minutes
	static unsigned int Register(PluginInterface *, unsigned int interval, unsigned int offsetH, unsigned int offsetM, Bunny * b, QVariant data = QVariant(), const char * callback = 0);
	// Will fire in interval minutes
	static unsigned int RegisterOneShot(PluginInterface *, unsigned int interval, Bunny * b, QVariant data = QVariant(), const char * callback = 0);
	// Will fire each day at time
	static unsigned int RegisterDaily(PluginInterface * p, QTime const& time, Bunny * b, QVariant data = QVariant(), const char * callback = 0);
	// Will fire each week at day:time
	static unsigned int RegisterWeekly(PluginInterface * p, Qt::DayOfWeek day, QTime const& time, Bunny * b, QVariant data = QVariant(), const char * callback = 0);
	static void Unregister(PluginInterface *, unsigned int id);
	static void UnregisterAllForBunny(PluginInterface *, Bunny *);
	static void UnregisterAll(PluginInterface *);

private slots:
	void OnTimer();
	
private:
	Cron();
	virtual ~Cron() {};
	static Cron& Instance();
	void AddCron(CronElement const&);
	unsigned int lastGivenID;
	QLinkedList<CronElement> CronElements;
};

#endif
