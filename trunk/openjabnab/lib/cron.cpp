#include <QDateTime>
#include <QTime>
#include <QTimer>
#include "cron.h"
#include "plugininterface.h"
#include "log.h"

Cron::Cron() {
	Log::Info("Cron Started...");
	// Conmpute next slot
	int now = QDateTime::currentDateTime().toTime_t();
	QTimer::singleShot(1000 * (60 - (now%60)), this, SLOT(OnTimer()));
	lastGivenID = 0;
}

void Cron::OnTimer()
{
	unsigned int now = QDateTime::currentDateTime().toTime_t();
	
	// Find elements to run
	while(!CronElements.isEmpty() && (CronElements.first().next_run <= now))
	{
		CronElement e = CronElements.takeFirst();

		if(e.callback)
			QMetaObject::invokeMethod(e.plugin, e.callback, Q_ARG(QVariant,e.data));
		else
			e.plugin->OnCron(e.data);

		e.next_run += e.interval;
		AddCron(e);
	}

	// Compute next slot
	now = QDateTime::currentDateTime().toTime_t();
	QTimer::singleShot(1000 * (60 - (now%60)), this, SLOT(OnTimer()));
}

unsigned int Cron::Register(PluginInterface * p, unsigned int interval, unsigned int offsetH, unsigned int offsetM, QVariant data, const char * callback)
{
	if(interval > 24*60)
	{
		Log::Error("Cron : Interval should be <= 1 day");
		return 0;
	}
	if(!interval)
	{
		Log::Error("Cron : Interval should be >= 1 minute");
		return 0;
	}
	if(!p)
	{
		Log::Error("Cron : pointer is null !");
		return 0;
	}
	
	Cron & theCron = Instance();
	unsigned id = ++theCron.lastGivenID;
	if(!id)
		Log::Error("Warning Cron::Register : lastGivenID overlapped !");

	CronElement e;
	e.interval = interval * 60;
	e.callback = callback;
	e.plugin = p;
	e.data = data;
	e.id = id;

	// Compute next run
	QDateTime now = QDateTime::currentDateTime();
	QDateTime time = now;
	time.addDays(-1);
	time.setTime(QTime(offsetH, offsetM));
	while(time < now)
		time = time.addSecs(interval*60);
	
	e.next_run = time.toTime_t();
	theCron.AddCron(e);

	Log::Debug(QString("Cron Register : %1 - %2").arg(p->GetVisualName(),time.toString()));
	return id;
}

unsigned int Cron::RegisterDaily(PluginInterface * p, QTime const& time, QVariant data, const char * callback)
{
	if(!p)
	{
		Log::Error("Cron : pointer is null !");
		return 0;
	}
	
	Cron & theCron = Instance();
	unsigned id = ++theCron.lastGivenID;
	if(!id)
		Log::Error("Warning Cron::Register : lastGivenID overlapped !");

	CronElement e;
	e.interval = 24 * 60 * 60; // DAILY
	e.callback = callback;
	e.plugin = p;
	e.data = data;
	e.id = id;

	// Compute next run
	QDateTime now = QDateTime::currentDateTime();
	QDateTime nextTime = now;
	nextTime.setTime(time);
	if(nextTime < now)
		nextTime = nextTime.addDays(1); // Tomorrow
	
	e.next_run = nextTime.toTime_t();
	theCron.AddCron(e);

	Log::Debug(QString("Cron Register : %1 - %2").arg(p->GetVisualName(),time.toString()));
	return id;
}

unsigned int Cron::RegisterWeekly(PluginInterface * p, Qt::DayOfWeek day, QTime const& time, QVariant data, const char * callback)
{
	if(!p)
	{
		Log::Error("Cron : pointer is null !");
		return 0;
	}
	
	Cron & theCron = Instance();
	unsigned id = ++theCron.lastGivenID;
	if(!id)
		Log::Error("Warning Cron::Register : lastGivenID overlapped !");

	CronElement e;
	e.interval = 7 * 24 * 60 * 60; // Weekly
	e.callback = callback;
	e.plugin = p;
	e.data = data;
	e.id = id;

	// Compute next run
	QDateTime now = QDateTime::currentDateTime();
	QDateTime nextTime = now;
	nextTime.setTime(time);
	nextTime = nextTime.addDays(day - now.date().dayOfWeek());
	if(nextTime < now)
		nextTime = nextTime.addDays(7); // Next week

	e.next_run = nextTime.toTime_t();
	theCron.AddCron(e);

	Log::Debug(QString("Cron Register : %1 - %2").arg(p->GetVisualName(),nextTime.toString()));
	return id;
}

void Cron::AddCron(CronElement const& e)
{
	QMutableLinkedListIterator<CronElement> i(CronElements);
	while(i.hasNext() && i.peekNext().next_run < e.next_run) // Find position
		i.next();
	i.insert(e);
}

void Cron::Unregister(PluginInterface * p, unsigned int id)
{
	Cron & theCron = Instance();
	QMutableLinkedListIterator<CronElement> i(theCron.CronElements);
	while(i.hasNext())
	{
		CronElement const& e = i.next();
		if(e.plugin == p && e.id == id)
		{
			i.remove();
			Log::Debug(QString("Cron Unregister : %1 - next %2").arg(p->GetVisualName(),QDateTime::fromTime_t(e.next_run).toString()));
		}
	}
}

void Cron::UnregisterAll(PluginInterface * p)
{
	Cron & theCron = Instance();
	QMutableLinkedListIterator<CronElement> i(theCron.CronElements);
	while(i.hasNext())
	{
		CronElement const& e = i.next();
		if(e.plugin == p)
			i.remove();
	}
}

Cron& Cron::Instance() {
  static Cron theCron;
  return theCron;
}
