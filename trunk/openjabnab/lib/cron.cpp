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
	Log::Info(QString("Cron Timer ") + QDateTime::currentDateTime().toString());
	unsigned int now = QDateTime::currentDateTime().toTime_t();
	
	for(QList<CronElement>::iterator it = CronElements.begin(); it != CronElements.end(); it++)
	{
		if(it->next_run <= now)
		{
			it->next_run += it->interval;
			if(it->callback)
			{
				QMetaObject::invokeMethod(it->plugin, it->callback, Q_ARG(QVariant,it->data));
			}
			else
			{
				it->plugin->OnCron(it->data);
			}
		}
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
	theCron.CronElements.append(e);

	Log::Info(QString("Cron Register : %1 - ").arg(p->GetVisualName()) + time.toString());
	return id;
}

void Cron::Unregister(PluginInterface * p, unsigned int id)
{
	Cron & theCron = Instance();
	QMutableListIterator<CronElement> i(theCron.CronElements);
	while(i.hasNext())
	{
		CronElement const& e = i.next();
		if(e.plugin == p && e.id == id)
			i.remove();
	}
}

void Cron::UnregisterAll(PluginInterface * p)
{
	Cron & theCron = Instance();
	QMutableListIterator<CronElement> i(theCron.CronElements);
	while(i.hasNext())
	{
		CronElement const& e = i.next();
		if(e.plugin == p)
			i.remove();
	}
}

QList<CronElement> Cron::GetByData(PluginInterface * p, QVariant v)
{
	QList<CronElement> list;
	Cron & theCron = Instance();
	QListIterator<CronElement> i(theCron.CronElements);
	while(i.hasNext())
	{
		CronElement const& e = i.next();
		if(e.plugin == p && e.data == v)
			list.append(e);
	}
	return list;
}

Cron& Cron::Instance() {
  static Cron theCron;
  return theCron;
}
