#include <QCoreApplication>
#include <QCryptographicHash>
#include <QUuid>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include "ambientpacket.h"
#include "messagepacket.h"
#include "choregraphy.h"
#include "bunny.h"
#include "log.h"
#include "httprequest.h"
#include "netdump.h"
#include "plugininterface.h"
#include "pluginmanager.h"
#include "sleeppacket.h"
#include "xmpphandler.h"
#include "account.h"

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

ApiManager::ApiAnswer * Bunny::ProcessVioletApiCall(HTTPRequest const& hRequest)
{
        ApiManager::ApiViolet* answer = new ApiManager::ApiViolet();

        QString serial = hRequest.GetArg("sn");
        QString token = hRequest.GetArg("token");

        if(true) // TODO: Check for good token
        {

                if(hRequest.GetURI().startsWith("/ojn/FR/api_stream.jsp"))
                {
                        if(hRequest.HasArg("urlList"))
                        {
				QByteArray message = ("ST " + hRequest.GetArg("urlList").split("|", QString::SkipEmptyParts).join("\nMW\nST ") + "\nMW\n").toAscii();
				SendPacket(MessagePacket(message));
                                answer->AddMessage("WEBRADIOSENT", "Your webradio has been sent");
                        }
                        else
                        {
                                answer->AddMessage("NOCORRECTPARAMETERS", "Please check urlList parameter !");
                        }
                }
                else
                {
                	AmbientPacket p;
                        if(hRequest.HasArg("action")) // TODO: send good values
                        {
                                switch(hRequest.GetArg("action").toInt())
                                {
                                        case 2:
                                                answer->AddXml("<listfriend nb=\"0\"/>");
                                                break;
                                        case 3:
                                                answer->AddXml("<listreceivedmsg nb=\"0\"/>");
                                                break;
                                        case 4:
                                                answer->AddXml("<timezone>(GMT + 01:00) Bruxelles, Copenhague, Madrid, Paris</timezone>");
                                                break;
                                        case 6:
                                                answer->AddXml("<blacklist nb=\"0\"/>");
                                                break;
                                        case 7:
                                                if(IsSleeping())
                                                        answer->AddXml("<rabbitSleep>YES</rabbitSleep>");
                                                else
                                                        answer->AddXml("<rabbitSleep>NO</rabbitSleep>");
                                                break;
                                        case 8:
                                                answer->AddXml("<rabbitVersion>V2</rabbitVersion>");
                                                break;
                                        case 9:
                                                answer->AddXml("<voiceListTTS nb=\"2\"/><voice lang=\"fr\" command=\"FR-Anastasie\"/><voice lang=\"de\" command=\"DE-Otto\"/>");
                                                break;
                                        case 10:
                                                answer->AddXml("<rabbitName>" + GetBunnyName() + "</rabbitName>");
                                                break;
                                        case 11:
                                                answer->AddXml("<langListUser nb=\"4\"/><myLang lang=\"fr\"/><myLang lang=\"us\"/><myLang lang=\"uk\"/><myLang lang=\"de\"/>");
                                                break;
                                        case 12:
                                                answer->AddXml("<message>LINKPREVIEW</message><comment>XXXX</comment>");
                                                break;
                                        case 13:
                                                answer->AddXml("<message>COMMANDSENT</message><comment>You rabbit will change status</comment>");
                                                break;
                                        case 14:
                                                answer->AddXml("<message>COMMANDSENT</message><comment>You rabbit will change status</comment>");
                                                break;
                                        default:
                                                break;
                                }
                        }
                        else
                        {
                                if(hRequest.HasArg("idmessage"))
                                {
                                        answer->AddMessage("MESSAGESENT", "Your message has been sent");
                                }
                                if(hRequest.HasArg("posleft") || hRequest.HasArg("posright"))
                                {
                                        int left = 0;
                                        int right = 0;
                                        if(hRequest.HasArg("posleft")) left = hRequest.GetArg("posleft").toInt();
                                        if(hRequest.HasArg("posright")) right = hRequest.GetArg("posright").toInt();
                                        if(left >= 0 && left <= 16 && right >= 0 && right <= 16)
                                        {
                                                answer->AddMessage("EARPOSITIONSENT", "Your ears command has been sent");
                                                p.SetEarsPosition(left, right);
                                        }
                                        else
                                        {
                                                answer->AddMessage("EARPOSITIONNOTSENT", "Your ears command could not be sent");
                                        }
                                }
                                if(hRequest.HasArg("tts"))
                                {
                                        answer->AddMessage("TTSSENT", "Your text has been sent");
                                }
                                if(hRequest.HasArg("ears"))
                                {
                                        answer->AddEarPosition(0, 0); // TODO: send real positions
                                }
                                if(hRequest.HasArg("chor"))
                                {
					Choregraphy c;
                                        if(c.Parse(hRequest.GetArg("chor"))) //TODO: Check for good chor
                                        {
						QDir chorFolder = QDir(GlobalSettings::GetString("Config/RealHttpRoot"));
						if (!chorFolder.cd("chor"))
						{
							if (!chorFolder.mkdir("chor"))
							{
								LogError(QString("Unable to create 'chor' directory !\n"));
                                                		answer->AddMessage("CHORNOTSENT", "Your chor could not be sent (can't create folder)");
							}
							chorFolder.cd("chor");
						}
						QString fileName = QCryptographicHash::hash(c.GetData(), QCryptographicHash::Md5).toHex().append(".chor");
						QString filePath = chorFolder.absoluteFilePath(fileName);

						QFile file(filePath);
						if (!file.open(QIODevice::WriteOnly))
						{
							LogError("Cannot open chor file for writing");
							answer->AddMessage("CHORNOTSENT", "Your chor could not be sent (error in file)");
						}
						else
						{
							file.write(c.GetData());
							file.close();
							SendPacket(MessagePacket(("CH broadcast/ojn_local/chor/" + fileName + "\n").toAscii()));
							answer->AddMessage("CHORSENT", "Your chor has been sent");
						}
                                        }
                                        else
                                        {
                                                answer->AddMessage("CHORNOTSENT", "Your chor could not be sent (bad chor)");
                                        }
                                }
                        }
			if(p.GetServices().count() > 0)
	                	SendPacket(p);
                }
        }
        else
        {
                answer->AddMessage("NOGOODTOKENORSERIAL", "Your token or serial number are not correct !");
        }
        return answer;
}

Bunny::~Bunny()
{
	SaveConfig();
}

QString Bunny::CheckPlugin(PluginInterface * plugin, bool isAssociated)
{
	if(!plugin)
		return QString("Unknown plugin : %1");

	if(plugin->GetType() != PluginInterface::BunnyPlugin && plugin->GetType() != PluginInterface::BunnyZtampPlugin)
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
			LogError(QString("Bunny %1 has invalid plugin (%2)!").arg(QString(GetID()), s));
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
		{
			singleClickPlugin = NULL;
			LogError(error.arg(pluginName));
		}
	}
	else
	{
		singleClickPlugin = NULL;
	}
	if(GlobalSettings.contains(DOUBLE_CLICK_PLUGIN_SETTINGNAME))
	{
		QString pluginName = GlobalSettings.value(DOUBLE_CLICK_PLUGIN_SETTINGNAME).toString();
		PluginInterface * plugin = PluginManager::Instance().GetPluginByName(pluginName);
		QString error = CheckPlugin(plugin, true);
		if(error.isNull())
			doubleClickPlugin = plugin;
		else
		{
			doubleClickPlugin = NULL;
			LogError(error.arg(pluginName));
		}
	}
	else
	{
		doubleClickPlugin = NULL;
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
	AmbientPacket a(AmbientPacket::Service_Nose, AmbientPacket::Nose_No);
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
	if(GlobalSettings.contains(SINGLE_CLICK_PLUGIN_SETTINGNAME))
	{
		QString pluginName = GlobalSettings.value(SINGLE_CLICK_PLUGIN_SETTINGNAME).toString();
		if(p->GetName() == pluginName)
		{
			QString error = CheckPlugin(p, true);
			if(error.isNull())
				singleClickPlugin = p;
			else
			{
				singleClickPlugin = NULL;
				LogError(error.arg(pluginName));
			}
		}
	}
	else
	{
		singleClickPlugin = NULL;
	}
	if(GlobalSettings.contains(DOUBLE_CLICK_PLUGIN_SETTINGNAME))
	{
		QString pluginName = GlobalSettings.value(DOUBLE_CLICK_PLUGIN_SETTINGNAME).toString();
		if(p->GetName() == pluginName)
		{
			QString error = CheckPlugin(p, true);
			if(error.isNull())
				doubleClickPlugin = p;
			else
			{
				doubleClickPlugin = NULL;
				LogError(error.arg(pluginName));
			}
		}
	}
	else
	{
		doubleClickPlugin = NULL;
	}
}

// API Remove plugin to this bunny
void Bunny::RemovePlugin(PluginInterface * p)
{
	if(listOfPlugins.contains(p->GetName()))
	{
		if(p == singleClickPlugin)
		{
			singleClickPlugin = NULL;
		}
		if(p == doubleClickPlugin)
		{
			doubleClickPlugin = NULL;
		}
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
		if(p->GetEnable())
			p->OnBunnyConnect(this);
	}
}

// Plugin unloaded
void Bunny::PluginUnloaded(PluginInterface * p)
{
	if(listOfPluginsPtr.contains(p))
	{
		listOfPluginsPtr.removeAll(p);
		if(p->GetEnable())
			p->OnBunnyDisconnect(this);
	}
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

void Bunny::InitApiCalls()
{
	DECLARE_API_CALL("registerPlugin(name)", &Bunny::Api_AddPlugin);
	DECLARE_API_CALL("unregisterPlugin(name)", &Bunny::Api_RemovePlugin);
	DECLARE_API_CALL("getListOfActivePlugins()", &Bunny::Api_GetListOfAssociatedPlugins);

	DECLARE_API_CALL("setSingleClickPlugin(name)", &Bunny::Api_SetSingleClickPlugin);
	DECLARE_API_CALL("setDoubleClickPlugin(name)", &Bunny::Api_SetDoubleClickPlugin);
	DECLARE_API_CALL("getClickPlugins()", &Bunny::Api_GetClickPlugins);

	DECLARE_API_CALL("getListOfKnownRFIDTags()", &Bunny::Api_GetListOfKnownRFIDTags);
	DECLARE_API_CALL("setRFIDTagName(tag,name)", &Bunny::Api_SetRFIDTagName);

	DECLARE_API_CALL("setBunnyName(name)", &Bunny::Api_SetBunnyName);

	DECLARE_API_CALL("setService(service,value)", &Bunny::Api_SetService);

	DECLARE_API_CALL("resetPassword()", &Bunny::Api_ResetPassword);
	DECLARE_API_CALL("resetOwner()", &Bunny::Api_ResetOwner);

	DECLARE_API_CALL("disconnect()", &Bunny::Api_Disconnect);

	DECLARE_API_CALL("enableVAPI()", &Bunny::Api_enableVApi);
	DECLARE_API_CALL("disableVAPI()", &Bunny::Api_disableVApi);
	DECLARE_API_CALL("getVAPIStatus()", &Bunny::Api_getVApiStatus);
	DECLARE_API_CALL("getVAPIToken()", &Bunny::Api_getVApiToken);
	DECLARE_API_CALL("setVAPIToken(tk)", &Bunny::Api_setVApiToken);
}

API_CALL(Bunny::Api_AddPlugin)
{
	Q_UNUSED(account);

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

	if(hRequest.GetArg("name") == "none")
	{
		RemoveGlobalSetting(SINGLE_CLICK_PLUGIN_SETTINGNAME);
		singleClickPlugin = NULL;
		return new ApiManager::ApiOk(QString("Removed preferred single click plugin"));
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

	QMap<QString, QVariant> list;

	QHash<QByteArray, QString>::const_iterator i;
	for (i = knownRFIDTags.constBegin(); i != knownRFIDTags.constEnd(); ++i)
		list.insert(QString(i.key()), i.value());

	return new ApiManager::ApiMappedList(list);
}

API_CALL(Bunny::Api_SetRFIDTagName)
{
	Q_UNUSED(account);

	QByteArray tagName = hRequest.GetArg("tag").toAscii();
	if(!knownRFIDTags.contains(tagName))
		return new ApiManager::ApiError(QString("Tag '%1' is unkown").arg(hRequest.GetArg("tag")));

	knownRFIDTags[tagName] = hRequest.GetArg("name");

	return new ApiManager::ApiOk(QString("Name '%1' associated to tag '%2'").arg(hRequest.GetArg("name"), hRequest.GetArg("tag")));
}

API_CALL(Bunny::Api_SetBunnyName)
{
	Q_UNUSED(account);

	SetBunnyName( hRequest.GetArg("name") );

	return new ApiManager::ApiOk(QString("Bunny '%1' is now named '%2'").arg(GetID(), hRequest.GetArg("name")));
}

API_CALL(Bunny::Api_SetService)
{
	Q_UNUSED(account);

	int service = hRequest.GetArg("service").toInt();
	int value = hRequest.GetArg("value").toInt();

	AmbientPacket a((AmbientPacket::Services)service, value);
	SendPacket(a);

	return new ApiManager::ApiOk(QString("Set value '%2' for service '%1'").arg(QString::number(service), QString::number(value)));
}

API_CALL(Bunny::Api_ResetPassword)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);

	ClearBunnyPassword();
	return new ApiManager::ApiOk("Password cleared");
}

API_CALL(Bunny::Api_ResetOwner)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);

	RemoveGlobalSetting("OwnerAccount");
	return new ApiManager::ApiOk("Owner cleared");
}

API_CALL(Bunny::Api_Disconnect)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);

        if(xmppHandler)
        {
                xmppHandler->Disconnect();
                xmppHandler = 0;
        }

	return new ApiManager::ApiOk("Connexion closed");
}

API_CALL(Bunny::Api_enableVApi)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);
	/* Get Token if exists */
	QString Token = GetGlobalSetting("VApiToken", "").toString();
	if(Token == "") {
		/* Generate random token */
		QByteArray Token = QCryptographicHash::hash(QUuid::createUuid().toString().toAscii(), QCryptographicHash::Md5).toHex();
		SetGlobalSetting("VApiToken",Token);
	}
	SetGlobalSetting("VApiEnable",true);
	return new ApiManager::ApiOk(QString("VioletAPI enabled"));
}

API_CALL(Bunny::Api_disableVApi)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);
	SetGlobalSetting("VApiEnable",false);
	return new ApiManager::ApiOk(QString("VioletAPI disabled"));
}

API_CALL(Bunny::Api_getVApiStatus)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);
	return new ApiManager::ApiString(GetGlobalSetting("VApiEnable", "false").toString());
}

API_CALL(Bunny::Api_getVApiToken)
{
	Q_UNUSED(account);
	Q_UNUSED(hRequest);
	return new ApiManager::ApiString(GetGlobalSetting("VApiToken", "").toString());
}

API_CALL(Bunny::Api_setVApiToken)
{
	if(!account.IsAdmin())
		return new ApiManager::ApiError("Access denied");

	SetGlobalSetting("VApiToken",hRequest.GetArg("tk").toAscii());
	return new ApiManager::ApiOk(QString("VioletAPI Token updated."));
}
