#include "cron.h"
#include "plugininterface.h"
#include "log.h"
#include <QTimer>
#include <QTime>
#include <QDateTime>

Cron::Cron() {
	Log::Info("Cron Started...");
	// Conmpute next slot
	int now = QDateTime::currentDateTime().toTime_t();
	QTimer::singleShot(1000 * (60 - (now%60)), this, SLOT(OnTimer()));
}

void Cron::OnTimer()
{
	Log::Info(QString("Cron Timer ") + QDateTime::currentDateTime().toString());
	unsigned int now = QDateTime::currentDateTime().toTime_t();
	
	for(QVector<CronElement>::iterator it = CronElements.begin(); it != CronElements.end(); it++)
	{
		if(it->next_run <= now)
		{
			//it->plugin->OnCron();
			it->next_run += it->interval;
			it->plugin->OnCron(it->id);
		}
	}
	
	// Compute next slot
	now = QDateTime::currentDateTime().toTime_t();
	QTimer::singleShot(1000 * (60 - (now%60)), this, SLOT(OnTimer()));
}

bool Cron::Register(PluginInterface * p, unsigned int interval, unsigned int offsetH, unsigned int offsetM, unsigned int id)
{
	if(interval > 24*60)
	{
		Log::Error("Cron : Interval should be <= 1 day");
		return false;
	}
	if(!interval)
	{
		Log::Error("Cron : Interval should be >= 1 minute");
		return false;
	}
	if(!p)
	{
		Log::Error("Cron : pointer is null !");
		return false;
	}
	
	CronElement e;
	e.interval = interval * 60;
	e.plugin = p;
	e.id = id;

	// Compute next run
	QDateTime now = QDateTime::currentDateTime();
	QDateTime time = now;
	time.addDays(-1);
	time.setTime(QTime(offsetH, offsetM));
	while(time < now)
		time = time.addSecs(interval*60);
	
	e.next_run = time.toTime_t();
	Instance().CronElements.append(e);

	Log::Info(QString("Cron Register : %1 - ").arg(p->GetVisualName()) + time.toString());
	return true;
}

void Cron::Unregister(PluginInterface * p, unsigned int id)
{
	Cron & theCron = Instance();
	QMutableVectorIterator<CronElement> i(theCron.CronElements);
	while(i.hasNext())
	{
		CronElement const& e = i.next();
		if(e.plugin == p && e.id == id)
			i.remove();
	}
	Log::Info(QString("Cron UnRegister : %1, id : %2").arg(p->GetVisualName()).arg(id));
}

void Cron::UnregisterAll(PluginInterface * p)
{
	Cron & theCron = Instance();
	QMutableVectorIterator<CronElement> i(theCron.CronElements);
	while(i.hasNext())
	{
		CronElement const& e = i.next();
		if(e.plugin == p)
			i.remove();
	}
	Log::Info(QString("Cron UnRegisterAll : %1").arg(p->GetVisualName()));
}

Cron& Cron::Instance() {
  static Cron theCron;
  return theCron;
}