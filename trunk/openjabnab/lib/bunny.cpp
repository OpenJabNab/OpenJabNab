#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include "bunny.h"
#include "log.h"
#include "httprequest.h"
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
	configFileName = bunniesDir.absoluteFilePath(bunnyID.toHex()+".dat");
	xmppHandler = 0;
	
	// Check if config file exists and load it
	if (QFile::exists(configFileName))
		LoadConfig();
		
	saveTimer = new QTimer(this);
	connect(saveTimer, SIGNAL(timeout()), this, SLOT(SaveConfig()));
	saveTimer->start(5*60*1000); // 5min
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
		Log::Error("Cannot open config file for reading : " + configFileName);
		return;
	}
	QDataStream in(&file);
	in.setVersion(QDataStream::Qt_4_3);
	in >> GlobalSettings >> PluginsSettings >> listOfPlugins;
	if (in.status() != QDataStream::Ok)
	{
		Log::Warning("Problem when loading config file for bunny : " + id.toHex());
	}
}

void Bunny::SaveConfig()
{
	QFile file(configFileName);
	if (!file.open(QIODevice::WriteOnly))
	{
		Log::Error("Cannot open config file for writing : " + configFileName);
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
	RegisterAllPlugins();
}

void Bunny::RemoveXmppHandler(XmppHandler * x)
{
	if (xmppHandler == x)
	{
		xmppHandler = 0;
		state = Disconnected;
	}
	UnregisterAllPlugins();
}

void Bunny::SendPacket(Packet const& p)
{
	if (xmppHandler)
		xmppHandler->WritePacketToBunny(p);
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
		if(IsConnected())
			p->RegisterBunny(this);
		SaveConfig();
	}
}

void Bunny::RemovePlugin(PluginInterface * p)
{
	if(listOfPlugins.contains(p->GetName()))
	{
		listOfPlugins.removeAll(p->GetName());
		if(IsConnected())
			p->RegisterBunny(this);
		SaveConfig();
	}
}

void Bunny::RegisterAllPlugins()
{
	foreach(QString s, listOfPlugins)
	{
		PluginInterface * p = PluginManager::Instance()->GetPluginByName(s);
		if(p)
			p->RegisterBunny(this);
		else
			Log::Error("Bunny " + GetID() + " has invalid plugin !");
	}
}

void Bunny::UnregisterAllPlugins()
{
	foreach(QString s, listOfPlugins)
	{
		PluginInterface * p = PluginManager::Instance()->GetPluginByName(s);
		if(p)
			p->UnregisterBunny(this);
		else
			Log::Error("Bunny " + GetID() + " has invalid plugin !");
	}
}

ApiManager::ApiAnswer * Bunny::ProcessApiCall(QByteArray const& functionName, HTTPRequest const& hRequest)
{
	if(functionName == "registerPlugin")
	{
		if(hRequest.HasArg("name"))
		{
			PluginInterface * plugin = PluginManager::Instance()->GetPluginByName(hRequest.GetArg("name"));
			if(!plugin)
				return new ApiManager::ApiError("Unknown plugin : " + hRequest.GetArg("name") + "<br />Request was : " + hRequest.toString());

			if(plugin->GetType() != PluginInterface::BunnyPlugin)
				return new ApiManager::ApiError("Bad plugin type : " + hRequest.GetArg("name") + "<br />Request was : " + hRequest.toString());

			AddPlugin(plugin);
			return new ApiManager::ApiString("Added " + plugin->GetVisualName() + " as active plugin.");
		}
		return new ApiManager::ApiError("Missing argument in Bunny Api Call 'RegisterPlugin' : " + hRequest.toString());
	}
	else if(functionName == "unregisterPlugin")
	{
		if(hRequest.HasArg("name"))
		{
			PluginInterface * plugin = PluginManager::Instance()->GetPluginByName(hRequest.GetArg("name"));
			if(!plugin)
				return new ApiManager::ApiError("Unknown plugin : " + hRequest.GetArg("name") + "<br />Request was : " + hRequest.toString());

			if(plugin->GetType() != PluginInterface::BunnyPlugin)
				return new ApiManager::ApiError("Bad plugin type : " + hRequest.GetArg("name") + "<br />Request was : " + hRequest.toString());

			RemovePlugin(plugin);
			return new ApiManager::ApiString("Removed " + plugin->GetVisualName() + " as active plugin.");
		}
		return new ApiManager::ApiError("Missing argument in Bunny Api Call 'UnregisterPlugin' : " + hRequest.toString());
	}
	else
	{
		return new ApiManager::ApiError("Unknown Bunny Api Call : " + hRequest.toString());
	}
}