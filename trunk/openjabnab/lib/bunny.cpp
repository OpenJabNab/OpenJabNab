#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include "bunny.h"
#include "log.h"
#include "httprequest.h"
#include "netdump.h"
#include "plugininterface.h"
#include "pluginmanager.h"
#include "xmpphandler.h"

Bunny::Bunny(QByteArray const& bunnyID)
{
	// Check bunnies folder
	QDir bunniesDir = QDir(QCoreApplication::applicationDirPath());
	if (!bunniesDir.cd("bunnies"))
	{
		if (!bunniesDir.mkdir("bunnies"))
		{
			Log::Error("Unable to create bunnies directory !\n");
			exit(-1);
		}
		bunniesDir.cd("bunnies");
	}
	id = bunnyID;
	state = Disconnected;
	isAuthenticated = false;
	configFileName = bunniesDir.absoluteFilePath(bunnyID.toHex()+".dat");
	xmppHandler = 0;
	
	// Check if config file exists and load it
	if (QFile::exists(configFileName))
		LoadConfig();
		
	saveTimer = new QTimer(this);
	connect(saveTimer, SIGNAL(timeout()), this, SLOT(SaveConfig()));
	saveTimer->start(5*60*1000); // 5min
}

void Bunny::InitApiCalls()
{
	DECLARE_API_CALL("registerPlugin", &Bunny::Api_RegisterPlugin);
	DECLARE_API_CALL("UnregisterPlugin", &Bunny::Api_UnregisterPlugin);
	DECLARE_API_CALL("getListOfActivePlugins", &Bunny::Api_GetListOfActivePlugins);
}

Bunny::~Bunny()
{
	SaveConfig();
}

void Bunny::LoadConfig()
{
	QFile file(configFileName);
	if (!file.open(QIODevice::ReadOnly))
	{
		Log::Error(QString("Cannot open config file for reading : %1").arg(configFileName));
		return;
	}
	QDataStream in(&file);
	in.setVersion(QDataStream::Qt_4_3);
	in >> GlobalSettings >> PluginsSettings >> listOfPlugins;
	if (in.status() != QDataStream::Ok)
	{
		Log::Warning(QString("Problem when loading config file for bunny : %1").arg(QString(id.toHex())));
	}
	foreach(QString s, listOfPlugins)
	{
		PluginInterface * p = PluginManager::Instance().GetPluginByName(s);
		if(p)
			listOfPluginsPtr.append(p);
		else
			Log::Error(QString("Bunny %1 has invalid plugin !").arg(QString(GetID())));
	}

}

void Bunny::SaveConfig()
{
	QFile file(configFileName);
	if (!file.open(QIODevice::WriteOnly))
	{
		Log::Error(QString("Cannot open config file for writing : %1").arg(configFileName));
		return;
	}
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_4_3);
	out << GlobalSettings << PluginsSettings << listOfPlugins;
}

void Bunny::SetXmppHandler(XmppHandler * x)
{ 
	state = Connected;
	xmppHandler = x;
	isAuthenticated = true;
	OnConnect();
}

void Bunny::RemoveXmppHandler(XmppHandler * x)
{
	if (xmppHandler == x)
	{
		xmppHandler = 0;
		state = Disconnected;
		isAuthenticated = false;
		OnDisconnect();
	}
}

void Bunny::SendPacket(Packet const& p)
{
	if (xmppHandler)
	{
		NetworkDump::Log("XMPP SendPacketToBunny", p.GetPrintableData());
		xmppHandler->WriteDataToBunny(p.GetData());
	}
}

void Bunny::SendData(QByteArray const& b)
{
	if (xmppHandler)
	{
		NetworkDump::Log("XMPP SendDataToBunny", b.toHex());
		xmppHandler->WriteDataToBunny(b);
	}
}

QVariant Bunny::GetGlobalSetting(QString const& key, QVariant const& defaultValue) const
{
	if (GlobalSettings.contains(key))
		return GlobalSettings.value(key);
	else
		return defaultValue;
}

void Bunny::SetGlobalSetting(QString const& key, QVariant const& value)
{
	GlobalSettings.insert(key, value);
}

QVariant Bunny::GetPluginSetting(QString const& pluginName, QString const& key, QVariant const& defaultValue) const
{
	if (PluginsSettings[pluginName].contains(key))
		return PluginsSettings[pluginName].value(key);
	else
		return defaultValue;
}

void Bunny::SetPluginSetting(QString const& pluginName, QString const& key, QVariant const& value)
{
	PluginsSettings[pluginName].insert(key, value);
}

void Bunny::AddPlugin(PluginInterface * p)
{
	if(!listOfPlugins.contains(p->GetName()))
	{
		listOfPlugins.append(p->GetName());
		listOfPluginsPtr.append(p);
		if(IsConnected() && p->GetEnable())
			p->OnBunnyConnect(this);
		SaveConfig();
	}
}

void Bunny::RemovePlugin(PluginInterface * p)
{
	if(listOfPlugins.contains(p->GetName()))
	{
		listOfPlugins.removeAll(p->GetName());
		listOfPluginsPtr.removeAll(p);
		if(IsConnected() && p->GetEnable())
			p->OnBunnyDisconnect(this);
		SaveConfig();
	}
}

void Bunny::OnConnect()
{
	// Send to all 'system' plugins
	PluginManager::Instance().OnBunnyConnect(this);
	// And all bunny's plugins
	foreach(PluginInterface * p, listOfPluginsPtr)
	{
		if(p->GetEnable())
			p->OnBunnyConnect(this);
	}
}

void Bunny::OnDisconnect()
{
	// Send to all 'system' plugins
	PluginManager::Instance().OnBunnyDisconnect(this);
	// And all bunny's plugins
	foreach(PluginInterface * p, listOfPluginsPtr)
	{
		if(p->GetEnable())
			p->OnBunnyDisconnect(this);
	}
}

void Bunny::XmppBunnyMessage(QByteArray const& data)
{
	// Send to all 'system' plugins
	PluginManager::Instance().XmppBunnyMessage(this, data);
	// And all bunny's plugins
	foreach(PluginInterface * p, listOfPluginsPtr)
	{
		if(p->GetEnable())
			p->XmppBunnyMessage(this, data);
	}
}

void Bunny::XmppVioletMessage(QByteArray const& data)
{
	// Send to all 'system' plugins
	PluginManager::Instance().XmppVioletMessage(this, data);
	// And all bunny's plugins
	foreach(PluginInterface * p, listOfPluginsPtr)
	{
		if(p->GetEnable())
			p->XmppVioletMessage(this, data);
	}
}

bool Bunny::XmppVioletPacketMessage(Packet const& packet)
{
	bool drop = PluginManager::Instance().XmppVioletPacketMessage(this, packet);
	foreach(PluginInterface * p, listOfPluginsPtr)
		if(p->GetEnable())
			drop |= p->XmppVioletPacketMessage(this, packet);
	return drop;
}

bool Bunny::OnClick(PluginInterface::ClickType type)
{
	if(PluginManager::Instance().OnClick(this, type))
		return true;

	// Call OnClick for all 'system' plugins until one returns true
	foreach(PluginInterface * p, listOfPluginsPtr)
	{
		if(p->GetEnable())
		{
			if(p->OnClick(this, type))
				return true;
		}
	}
	return false;
}

bool Bunny::OnEarsMove(int left, int right)
{
	if(PluginManager::Instance().OnEarsMove(this, left, right))
		return true;

	// Call OnClick for all 'system' plugins until one returns true
	foreach(PluginInterface * p, listOfPluginsPtr)
	{
		if(p->GetEnable())
		{
			if(p->OnEarsMove(this, left, right))
				return true;
		}
	}
	return false;
}

bool Bunny::OnRFID(QByteArray const& tag)
{
	if(PluginManager::Instance().OnRFID(this, tag))
		return true;

	// Call OnClick for all 'system' plugins until one returns true
	foreach(PluginInterface * p, listOfPluginsPtr)
	{
		if(p->GetEnable())
		{
			if(p->OnRFID(this, tag))
				return true;
		}
	}
	return false;
}

void Bunny::PluginStateChanged(PluginInterface * p)
{
	if(listOfPluginsPtr.contains(p) && state == Connected)
	{
		if(p->GetEnable())
			p->OnBunnyConnect(this);
		else
			p->OnBunnyDisconnect(this);
	}
}

void Bunny::PluginLoaded(PluginInterface * p)
{
	if(listOfPlugins.contains(p->GetName()))
	{
		listOfPluginsPtr.append(p);
		if(p->GetEnable() && state == Connected)
			p->OnBunnyConnect(this);
	}
}

void Bunny::PluginUnloaded(PluginInterface * p)
{
	if(listOfPluginsPtr.contains(p))
		listOfPluginsPtr.removeAll(p);
}

API_CALL(Bunny::Api_RegisterPlugin)
{
	Q_UNUSED(account);

	if(!hRequest.HasArg("name"))
		return new ApiManager::ApiError(QString("Missing argument in Bunny Api Call 'RegisterPlugin' : %1").arg(hRequest.toString()));

	PluginInterface * plugin = PluginManager::Instance().GetPluginByName(hRequest.GetArg("name"));
	if(!plugin)
		return new ApiManager::ApiError(QString("Unknown plugin : %1<br />Request was : %2").arg(hRequest.GetArg("name"),hRequest.toString()));

	if(plugin->GetType() != PluginInterface::BunnyPlugin)
		return new ApiManager::ApiError(QString("Bad plugin type : %1<br />Request was : %2").arg(hRequest.GetArg("name"),hRequest.toString()));

	AddPlugin(plugin);
	return new ApiManager::ApiOk(QString("Added '%1' as active plugin").arg(plugin->GetVisualName()));
}

API_CALL(Bunny::Api_UnregisterPlugin)
{
	Q_UNUSED(account);

	if(!hRequest.HasArg("name"))
		return new ApiManager::ApiError(QString("Missing argument in Bunny Api Call 'UnregisterPlugin' : %1").arg(hRequest.toString()));

	PluginInterface * plugin = PluginManager::Instance().GetPluginByName(hRequest.GetArg("name"));
	if(!plugin)
		return new ApiManager::ApiError(QString("Unknown plugin : %1<br />Request was : %2").arg(hRequest.GetArg("name"),hRequest.toString()));

	if(plugin->GetType() != PluginInterface::BunnyPlugin)
		return new ApiManager::ApiError(QString("Bad plugin type : %1<br />Request was : %2").arg(hRequest.GetArg("name"),hRequest.toString()));

	RemovePlugin(plugin);
	return new ApiManager::ApiOk(QString("Removed '%1' as active plugin").arg(plugin->GetVisualName()));
}

API_CALL(Bunny::Api_GetListOfActivePlugins)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);

	QList<QString> list;
	foreach (PluginInterface * p, listOfPluginsPtr)
		list.append(p->GetName());

	return new ApiManager::ApiList(list);

}
