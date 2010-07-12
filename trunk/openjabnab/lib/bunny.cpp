#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include "ambientpacket.h"
#include "bunny.h"
#include "log.h"
#include "httprequest.h"
#include "netdump.h"
#include "plugininterface.h"
#include "pluginmanager.h"
#include "sleeppacket.h"
#include "xmpphandler.h"

#define SINGLE_CLICK_PLUGIN_SETTINGNAME "singleClickPlugin"
#define DOUBLE_CLICK_PLUGIN_SETTINGNAME "doubleClickPlugin"

Bunny::Bunny(QByteArray const& bunnyID)
{
	// Check bunnies folder
	QDir bunniesDir = QDir(QCoreApplication::applicationDirPath());
	if (!bunniesDir.cd("bunnies"))
	{
		if (!bunniesDir.mkdir("bunnies"))
		{
			LogError("Unable to create bunnies directory !\n");
			exit(-1);
		}
		bunniesDir.cd("bunnies");
	}
	id = bunnyID;
	state = State_Disconnected;
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
	DECLARE_API_CALL("registerPlugin", &Bunny::Api_AddPlugin);
	DECLARE_API_CALL("unregisterPlugin", &Bunny::Api_RemovePlugin);
	DECLARE_API_CALL("getListOfActivePlugins", &Bunny::Api_GetListOfAssociatedPlugins);

	DECLARE_API_CALL("setSingleClickPlugin", &Bunny::Api_SetSingleClickPlugin);
	DECLARE_API_CALL("setDoubleClickPlugin", &Bunny::Api_SetDoubleClickPlugin);
	DECLARE_API_CALL("getClickPlugins", &Bunny::Api_GetClickPlugins);

	DECLARE_API_CALL("getListOfKnownRFIDTags", &Bunny::Api_GetListOfKnownRFIDTags);
	DECLARE_API_CALL("setRFIDTagName", &Bunny::Api_SetRFIDTagName);
}

Bunny::~Bunny()
{
	SaveConfig();
}

QString Bunny::CheckPlugin(PluginInterface * plugin, bool isAssociated)
{
	if(!plugin)
		return QString("Unknown plugin : %1");

	if(plugin->GetType() != PluginInterface::BunnyPlugin)
		return QString("Bad plugin type : %1");
		
	if(!plugin->GetEnable())
		return QString("Plugin '%1' is globally disabled");
		
	if(isAssociated && (!listOfPluginsPtr.contains(plugin)))
		return QString("Plugin '%1' is not associated with this bunny");
		
	return QString();
}


void Bunny::LoadConfig()
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
		LogWarning(QString("Problem when loading config file for bunny : %1").arg(QString(id.toHex())));
	}
	
	// "Load" associated bunny plugins
	foreach(QString s, listOfPlugins)
	{
		PluginInterface * p = PluginManager::Instance().GetPluginByName(s);
		if(p)
		{
			listOfPluginsPtr.append(p);
			if(!p->GetEnable())
				LogWarning(QString("Bunny %1 : '%2' is globally disabled !").arg(QString(GetID()), s));
		}
		else
			LogError(QString("Bunny %1 has invalid plugin !").arg(QString(GetID())));
	}
	
	// Load single/doubleClickPlugin preferences
	if(GlobalSettings.contains(SINGLE_CLICK_PLUGIN_SETTINGNAME))
	{
		QString pluginName = GlobalSettings.value(SINGLE_CLICK_PLUGIN_SETTINGNAME).toString();
		PluginInterface * plugin = PluginManager::Instance().GetPluginByName(pluginName);
		QString error = CheckPlugin(plugin, true);
		if(error.isNull())
			singleClickPlugin = plugin;
		else
			LogError(error.arg(pluginName));
	}
	if(GlobalSettings.contains(DOUBLE_CLICK_PLUGIN_SETTINGNAME))
	{
		QString pluginName = GlobalSettings.value(DOUBLE_CLICK_PLUGIN_SETTINGNAME).toString();
		PluginInterface * plugin = PluginManager::Instance().GetPluginByName(pluginName);
		QString error = CheckPlugin(plugin, true);
		if(error.isNull())
			doubleClickPlugin = plugin;
		else
			LogError(error.arg(pluginName));
	}
	
	// Added to config file, listOfRFIDTags
	if(!in.atEnd())
	{
		// Load Known RFID Tags
		in >> knownRFIDTags;
	}
}

void Bunny::SaveConfig()
{
	QFile file(configFileName);
	if (!file.open(QIODevice::WriteOnly))
	{
		LogError(QString("Cannot open config file for writing : %1").arg(configFileName));
		return;
	}
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_4_3);
	out << GlobalSettings << PluginsSettings << listOfPlugins << knownRFIDTags;
}

void Bunny::SetXmppHandler(XmppHandler * x)
{ 
	xmppHandler = x;
}

void Bunny::RemoveXmppHandler(XmppHandler * x)
{
	if (xmppHandler == x)
	{
		xmppHandler = 0;
		state = State_Disconnected;
		OnDisconnect();
	}
}

void Bunny::Booting()
{
	state = State_Booting;
	if(xmppHandler)
	{
		xmppHandler->Disconnect();
		xmppHandler = 0;
	}
	SetGlobalSetting("Last BootRequest", QDateTime::currentDateTime());
}

// Called when the bunny start an authenticating process
void Bunny::Authenticating()
{
	if(xmppHandler)
	{
		xmppHandler->Disconnect();
		xmppHandler = 0;
	}
	state = State_Authenticating;
}

// Called when the bunny succeed an auth
void Bunny::Authenticated()
{
	state = State_Authenticated;
	SetGlobalSetting("Last JabberConnection", QDateTime::currentDateTime());
}

// Called when the bunny is ready (auth/boot finished)
void Bunny::Ready()
{
	state = State_Ready;
	OnConnect();
}

// Called when the bunny is requesting init packet (during boot)
QByteArray Bunny::GetInitPacket() const
{
	// Create minimal packet
	AmbientPacket a(AmbientPacket::Service_Noze, AmbientPacket::Noze_No);
	a.SetEarsPosition(0,0);

	SleepPacket s(SleepPacket::Wake_Up);
	
	// Pass AmbientPacket to all bunny's plugins
	foreach(PluginInterface * p, listOfPluginsPtr)
	{
		if(p->GetEnable())
			p->OnInitPacket(this, a, s);
	}
	
	// Create packetList and return packet's data
	QList<Packet *> l;
	l.append(&a);
	l.append(&s);
	
	return Packet::GetData(l);
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

void Bunny::RemoveGlobalSetting(QString const& key)
{
	GlobalSettings.remove(key);
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

void Bunny::RemovePluginSetting(QString const& pluginName, QString const& key)
{
	PluginsSettings[pluginName].remove(key);
}

// API Add plugin to this bunny
void Bunny::AddPlugin(PluginInterface * p)
{
	if(!listOfPlugins.contains(p->GetName()))
	{
		listOfPlugins.append(p->GetName());
		listOfPluginsPtr.append(p);
		if(IsConnected())
			p->OnBunnyConnect(this);
		SaveConfig();
	}
}

// API Remove plugin to this bunny
void Bunny::RemovePlugin(PluginInterface * p)
{
	if(listOfPlugins.contains(p->GetName()))
	{
		listOfPlugins.removeAll(p->GetName());
		listOfPluginsPtr.removeAll(p);
		if(IsConnected())
			p->OnBunnyDisconnect(this);
		SaveConfig();
	}
}

// Global plugin enable/disable
void Bunny::PluginStateChanged(PluginInterface * p)
{
	if(listOfPluginsPtr.contains(p) && IsConnected())
	{
		if(p->GetEnable())
			p->OnBunnyConnect(this);
		else
			p->OnBunnyDisconnect(this);
	}
}

// New plugin loaded
void Bunny::PluginLoaded(PluginInterface * p)
{
	if(listOfPlugins.contains(p->GetName()))
	{
		listOfPluginsPtr.append(p);
		if(p->GetEnable() && IsConnected())
			p->OnBunnyConnect(this);
	}
}

// Plogin unloaded
void Bunny::PluginUnloaded(PluginInterface * p)
{
	if(listOfPluginsPtr.contains(p))
		listOfPluginsPtr.removeAll(p);
}

// Bunny is connected
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

// Bunny is gone away
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
	SaveConfig();
}

// Received XMPP Message
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

// Received XMPP Message from Violet (proxy mode)
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

// Received <packet> from Violet (proxy mode)
bool Bunny::XmppVioletPacketMessage(Packet const& packet)
{
	bool drop = PluginManager::Instance().XmppVioletPacketMessage(this, packet);
	foreach(PluginInterface * p, listOfPluginsPtr)
		if(p->GetEnable())
			drop |= p->XmppVioletPacketMessage(this, packet);
	return drop;
}

// Called when top button is pushed
bool Bunny::OnClick(PluginInterface::ClickType type)
{
	if(PluginManager::Instance().OnClick(this, type))
		return true;

	// Check if registeredClickPlugin is available
	if(type == PluginInterface::SingleClick && singleClickPlugin)
	{
		return singleClickPlugin->OnClick(this, type);
	}
	if(type == PluginInterface::DoubleClick && doubleClickPlugin)
	{
		return doubleClickPlugin->OnClick(this, type);
	}
	return false;
}

// Called when ears was moded
bool Bunny::OnEarsMove(int left, int right)
{
	if(PluginManager::Instance().OnEarsMove(this, left, right))
		return true;

	// Call OnClick for all 'bunny' plugins until one returns true
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

// Called when a RFID Tad was read
bool Bunny::OnRFID(QByteArray const& tag)
{
	if(!knownRFIDTags.contains(tag))
		knownRFIDTags.insert(tag, QString());
	
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

/*******/
/* API */
/*******/

API_CALL(Bunny::Api_AddPlugin)
{
	Q_UNUSED(account);

	if(!hRequest.HasArg("name"))
		return new ApiManager::ApiError(QString("Missing argument in Bunny Api Call 'RegisterPlugin' : %1").arg(hRequest.toString()));

	PluginInterface * plugin = PluginManager::Instance().GetPluginByName(hRequest.GetArg("name"));
	
	QString error = CheckPlugin(plugin);
	if(!error.isNull())
		return new ApiManager::ApiError(error.arg(hRequest.GetArg("name")));

	AddPlugin(plugin);
	return new ApiManager::ApiOk(QString("Added '%1' as active plugin").arg(plugin->GetVisualName()));
}

API_CALL(Bunny::Api_RemovePlugin)
{
	Q_UNUSED(account);

	if(!hRequest.HasArg("name"))
		return new ApiManager::ApiError(QString("Missing argument in Bunny Api Call 'UnregisterPlugin' : %1").arg(hRequest.toString()));

	PluginInterface * plugin = PluginManager::Instance().GetPluginByName(hRequest.GetArg("name"));
	QString error = CheckPlugin(plugin);
	if(!error.isNull())
		return new ApiManager::ApiError(error.arg(hRequest.GetArg("name")));

	RemovePlugin(plugin);
	return new ApiManager::ApiOk(QString("Removed '%1' as active plugin").arg(plugin->GetVisualName()));
}

API_CALL(Bunny::Api_GetListOfAssociatedPlugins)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);

	QList<QString> list;
	foreach (PluginInterface * p, listOfPluginsPtr)
		list.append(p->GetName());

	return new ApiManager::ApiList(list);

}

API_CALL(Bunny::Api_SetSingleClickPlugin)
{
	Q_UNUSED(account);

	if(!hRequest.HasArg("name"))
		return new ApiManager::ApiError(QString("Missing argument in Bunny Api Call 'SetSingleClickPlugin' : %1").arg(hRequest.toString()));

	if(hRequest.GetArg("name") == "none")
	{
		RemoveGlobalSetting(SINGLE_CLICK_PLUGIN_SETTINGNAME);
		singleClickPlugin = NULL;
		return new ApiManager::ApiOk(QString("Removed preferred double click plugin"));
	}

	PluginInterface * plugin = PluginManager::Instance().GetPluginByName(hRequest.GetArg("name"));

	QString error = CheckPlugin(plugin, true);
	if(!error.isNull())
		return new ApiManager::ApiError(error.arg(hRequest.GetArg("name")));

	singleClickPlugin = plugin;
	SetGlobalSetting(SINGLE_CLICK_PLUGIN_SETTINGNAME, plugin->GetName());
	return new ApiManager::ApiOk(QString("Set '%1' as single click plugin").arg(plugin->GetVisualName()));
}

API_CALL(Bunny::Api_SetDoubleClickPlugin)
{
	Q_UNUSED(account);

	if(!hRequest.HasArg("name"))
		return new ApiManager::ApiError(QString("Missing argument in Bunny Api Call 'SetSingleClickPlugin' : %1").arg(hRequest.toString()));
		
	if(hRequest.GetArg("name") == "none")
	{
		RemoveGlobalSetting(DOUBLE_CLICK_PLUGIN_SETTINGNAME);
		doubleClickPlugin = NULL;
		return new ApiManager::ApiOk(QString("Removed preferred double click plugin"));
	}
	
	PluginInterface * plugin = PluginManager::Instance().GetPluginByName(hRequest.GetArg("name"));

	QString error = CheckPlugin(plugin, true);
	if(!error.isNull())
		return new ApiManager::ApiError(error.arg(hRequest.GetArg("name")));

	doubleClickPlugin = plugin;
	SetGlobalSetting(DOUBLE_CLICK_PLUGIN_SETTINGNAME, plugin->GetName());
	return new ApiManager::ApiOk(QString("Set '%1' as double click plugin").arg(plugin->GetVisualName()));
}

API_CALL(Bunny::Api_GetClickPlugins)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);

	QList<QString> list;
	list.append(GetGlobalSetting(SINGLE_CLICK_PLUGIN_SETTINGNAME, QString()).toString());
	list.append(GetGlobalSetting(DOUBLE_CLICK_PLUGIN_SETTINGNAME, QString()).toString());

	return new ApiManager::ApiList(list);
}

API_CALL(Bunny::Api_GetListOfKnownRFIDTags)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);
	
	QMap<QString, QString> list;

	QHash<QByteArray, QString>::const_iterator i;
	for (i = knownRFIDTags.constBegin(); i != knownRFIDTags.constEnd(); ++i)
		list.insert(QString(i.key()), i.value());
	
	return new ApiManager::ApiMappedList(list);
}

API_CALL(Bunny::Api_SetRFIDTagName)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);

	if(!hRequest.HasArg("tag"))
		return new ApiManager::ApiError(QString("Missing 'tag' argument in Bunny Api Call 'SetRFIDTagName' : %1").arg(hRequest.toString()));

	if(!hRequest.HasArg("name"))
		return new ApiManager::ApiError(QString("Missing 'name' argument in Bunny Api Call 'SetRFIDTagName' : %1").arg(hRequest.toString()));
	
	QByteArray tagName = hRequest.GetArg("tag").toAscii();
	if(!knownRFIDTags.contains(tagName))
		return new ApiManager::ApiError(QString("Tag '%1' is unkown").arg(hRequest.GetArg("tag")));
		
	knownRFIDTags[tagName] = hRequest.GetArg("name");
	
	return new ApiManager::ApiOk(QString("Name '%1' associated to tag '%2'").arg(hRequest.GetArg("name"), hRequest.GetArg("tag")));
}
