#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include "ambientpacket.h"
#include "ztamp.h"
#include "bunny.h"
#include "log.h"
#include "httprequest.h"
#include "netdump.h"
#include "plugininterface.h"
#include "pluginmanager.h"
#include "sleeppacket.h"
#include "xmpphandler.h"

Ztamp::Ztamp(QByteArray const& ztampID)
{
	// Check ztamps folder
	QDir ztampsDir = QDir(QCoreApplication::applicationDirPath());
	if (!ztampsDir.cd("ztamps"))
	{
		if (!ztampsDir.mkdir("ztamps"))
		{
			LogError("Unable to create ztamps directory !\n");
			exit(-1);
		}
		ztampsDir.cd("ztamps");
	}
	id = ztampID;
	configFileName = ztampsDir.absoluteFilePath(ztampID.toHex()+".dat");

	// Check if config file exists and load it
	if (QFile::exists(configFileName))
		LoadConfig();

	saveTimer = new QTimer(this);
	connect(saveTimer, SIGNAL(timeout()), this, SLOT(SaveConfig()));
	saveTimer->start(5*60*1000); // 5min
}

Ztamp::~Ztamp()
{
	SaveConfig();
}

QString Ztamp::CheckPlugin(PluginInterface * plugin, bool isAssociated)
{
	if(!plugin)
		return QString("Unknown plugin : %1");

	if(plugin->GetType() != PluginInterface::ZtampPlugin && plugin->GetType() != PluginInterface::BunnyZtampPlugin)
		return QString("Bad plugin type : %1");

	if(!plugin->GetEnable())
		return QString("Plugin '%1' is globally disabled");

	if(isAssociated && (!listOfPluginsPtr.contains(plugin)))
		return QString("Plugin '%1' is not associated with this ztamp");

	return QString();
}


void Ztamp::LoadConfig()
{
	QFile file(configFileName);
	if (!file.open(QIODevice::ReadOnly))
	{
		LogError(QString("Cannot open config file for reading : %1").arg(configFileName));
		return;
	}

	QDataStream in(&file);
	in.setVersion(QDataStream::Qt_4_3);
	in >> GlobalSettings >> PluginsSettings >> listOfPlugins;
	if (in.status() != QDataStream::Ok)
	{
		LogWarning(QString("Problem when loading config file for ztamp : %1").arg(QString(id.toHex())));
	}

	// "Load" associated ztamp plugins
	foreach(QString s, listOfPlugins)
	{
		PluginInterface * p = PluginManager::Instance().GetPluginByName(s);
		if(p)
		{
			listOfPluginsPtr.append(p);
			if(!p->GetEnable())
				LogWarning(QString("Ztamp %1 : '%2' is globally disabled !").arg(QString(GetID()), s));
		}
		else
			LogError(QString("Ztamp %1 has invalid plugin (%2)!").arg(QString(GetID()), s));
	}
}

void Ztamp::SaveConfig()
{
	QFile file(configFileName);
	if (!file.open(QIODevice::WriteOnly))
	{
		LogError(QString("Cannot open config file for writing : %1").arg(configFileName));
		return;
	}
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_4_3);
	out << GlobalSettings << PluginsSettings << listOfPlugins;// << knownRFIDTags;
}

QVariant Ztamp::GetGlobalSetting(QString const& key, QVariant const& defaultValue) const
{
	if (GlobalSettings.contains(key))
		return GlobalSettings.value(key);
	else
		return defaultValue;
}

void Ztamp::SetGlobalSetting(QString const& key, QVariant const& value)
{
	GlobalSettings.insert(key, value);
}

void Ztamp::RemoveGlobalSetting(QString const& key)
{
	GlobalSettings.remove(key);
}

QVariant Ztamp::GetPluginSetting(QString const& pluginName, QString const& key, QVariant const& defaultValue) const
{
	if (PluginsSettings[pluginName].contains(key))
		return PluginsSettings[pluginName].value(key);
	else
		return defaultValue;
}

void Ztamp::SetPluginSetting(QString const& pluginName, QString const& key, QVariant const& value)
{
	PluginsSettings[pluginName].insert(key, value);
}

void Ztamp::RemovePluginSetting(QString const& pluginName, QString const& key)
{
	PluginsSettings[pluginName].remove(key);
}

// API Add plugin to this ztamp
void Ztamp::AddPlugin(PluginInterface * p)
{
	if(!listOfPlugins.contains(p->GetName()))
	{
		listOfPlugins.append(p->GetName());
		listOfPluginsPtr.append(p);
		p->OnZtampConnect(this);
		SaveConfig();
	}
}

// API Remove plugin to this ztamp
void Ztamp::RemovePlugin(PluginInterface * p)
{
	if(listOfPlugins.contains(p->GetName()))
	{
		listOfPlugins.removeAll(p->GetName());
		listOfPluginsPtr.removeAll(p);
		p->OnZtampDisconnect(this);
		SaveConfig();
	}
}

// Global plugin enable/disable
void Ztamp::PluginStateChanged(PluginInterface * p)
{
	if(listOfPluginsPtr.contains(p))
	{
		if(p->GetEnable())
			p->OnZtampConnect(this);
		else
			p->OnZtampDisconnect(this);
	}
}

// New plugin loaded
void Ztamp::PluginLoaded(PluginInterface * p)
{
	if(listOfPlugins.contains(p->GetName()))
	{
		listOfPluginsPtr.append(p);
		if(p->GetEnable())
			p->OnZtampConnect(this);
	}
}

// Plogin unloaded
void Ztamp::PluginUnloaded(PluginInterface * p)
{
	if(listOfPluginsPtr.contains(p))
	{
		listOfPluginsPtr.removeAll(p);
		if(p->GetEnable())
			p->OnZtampDisconnect(this);
	}
}

// Ztamp is connected
void Ztamp::OnConnect()
{
	// Send to all 'system' plugins
	PluginManager::Instance().OnZtampConnect(this);

	// And all ztamp's plugins
	foreach(PluginInterface * p, listOfPluginsPtr)
	{
		if(p->GetEnable())
			p->OnZtampConnect(this);
	}
}

// Ztamp is gone away
void Ztamp::OnDisconnect()
{
	// Send to all 'system' plugins
	PluginManager::Instance().OnZtampDisconnect(this);

	// And all ztamp's plugins
	foreach(PluginInterface * p, listOfPluginsPtr)
	{
		if(p->GetEnable())
			p->OnZtampDisconnect(this);
	}
	SaveConfig();
}

// Called when a RFID Tad was read
bool Ztamp::OnRFID(Bunny * bunny)
{
	if(PluginManager::Instance().OnRFID(this, bunny))
		return true;

	// Call OnClick for all 'system' plugins until one returns true
	foreach(PluginInterface * p, listOfPluginsPtr)
	{
		if(p->GetEnable())
		{
			if(p->OnRFID(this, bunny))
				return true;
		}
	}
	return false;
}

/*******/
/* API */
/*******/

void Ztamp::InitApiCalls()
{
	DECLARE_API_CALL("registerPlugin(name)", &Ztamp::Api_AddPlugin);
	DECLARE_API_CALL("unregisterPlugin(name)", &Ztamp::Api_RemovePlugin);
	DECLARE_API_CALL("getListOfActivePlugins()", &Ztamp::Api_GetListOfAssociatedPlugins);
	DECLARE_API_CALL("setZtampName(name)", &Ztamp::Api_SetZtampName);
	DECLARE_API_CALL("removeOwner(login)", &Ztamp::Api_RemoveOwner);
	DECLARE_API_CALL("resetOwner()", &Ztamp::Api_ResetOwner);
}

API_CALL(Ztamp::Api_AddPlugin)
{
	Q_UNUSED(account);

	PluginInterface * plugin = PluginManager::Instance().GetPluginByName(hRequest.GetArg("name"));

	QString error = CheckPlugin(plugin);
	if(!error.isNull())
		return new ApiManager::ApiError(error.arg(hRequest.GetArg("name")));

	AddPlugin(plugin);
	return new ApiManager::ApiOk(QString("Added '%1' as active plugin").arg(plugin->GetVisualName()));
}

API_CALL(Ztamp::Api_RemovePlugin)
{
	Q_UNUSED(account);

	PluginInterface * plugin = PluginManager::Instance().GetPluginByName(hRequest.GetArg("name"));
	QString error = CheckPlugin(plugin);
	if(!error.isNull())
		return new ApiManager::ApiError(error.arg(hRequest.GetArg("name")));

	RemovePlugin(plugin);
	return new ApiManager::ApiOk(QString("Removed '%1' as active plugin").arg(plugin->GetVisualName()));
}

API_CALL(Ztamp::Api_GetListOfAssociatedPlugins)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);

	QList<QString> list;
	foreach (PluginInterface * p, listOfPluginsPtr)
		list.append(p->GetName());

	return new ApiManager::ApiList(list);

}

API_CALL(Ztamp::Api_SetZtampName)
{
	Q_UNUSED(account);

	SetZtampName( hRequest.GetArg("name") );

	return new ApiManager::ApiOk(QString("Ztamp '%1' is now named '%2'").arg(GetID(), hRequest.GetArg("name")));
}

API_CALL(Ztamp::Api_RemoveOwner)
{
	Q_UNUSED(account);

	QString owner = hRequest.GetArg("login");
	if(owner == "")
		return new ApiManager::ApiError("Bad login");

	QStringList owners = GetGlobalSetting("OwnerAccounts","").toStringList();
	owners.removeAll(owner);
	SetGlobalSetting("OwnerAccounts", owners);
	return new ApiManager::ApiOk(QString("Owner '%1' removed").arg(owner));
}

API_CALL(Ztamp::Api_ResetOwner)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);

	RemoveGlobalSetting("OwnerAccounts");
	return new ApiManager::ApiOk("Owner cleared");
}
