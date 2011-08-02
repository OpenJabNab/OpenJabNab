#include <QCoreApplication>
#include <QDir>
#include <QLibrary>
#include <QPluginLoader>
#include <QString>
#include "apimanager.h"
#include "account.h"
#include "httprequest.h"
#include "log.h"
#include "pluginmanager.h"
#include <iostream>

PluginManager::PluginManager()
{
	// Load all plugins
	pluginsDir = QCoreApplication::applicationDirPath();
	pluginsDir.cd("plugins");
}

PluginManager & PluginManager::Instance()
{
  static PluginManager p;
  return p;
}

void PluginManager::UnloadPlugins()
{
	foreach(PluginInterface * p, listOfPlugins)
		delete p;

	foreach(QPluginLoader * l, listOfPluginsLoader.values())
	{
		l->unload();
		delete l;
	}

}

int PluginManager::GetEnabledPluginCount()
{
	int active = 0;
	foreach(PluginInterface * plugin, listOfPlugins)
		if(plugin->GetEnable())
			active++;
	return active;
}

int PluginManager::GetPluginCount()
{
	return listOfPlugins.count();
}

void PluginManager::LoadPlugins()
{
	LogInfo(QString("Finding plugins in : %1").arg(pluginsDir.path()));
	foreach (QString fileName, pluginsDir.entryList(QDir::Files))
		LoadPlugin(fileName);
}

bool PluginManager::LoadPlugin(QString const& fileName)
{
	if(listOfPluginsByFileName.contains(fileName))
	{
		LogError(QString("Plugin '%1' already loaded !").arg(fileName));
		return false;
	}

	QString file = pluginsDir.absoluteFilePath(fileName);
	if (!QLibrary::isLibrary(file))
		return false;

	QString status = QString("Loading %1 : ").arg(fileName);

	QPluginLoader * loader = new QPluginLoader(file);
	QObject * p = loader->instance();
	PluginInterface * plugin = qobject_cast<PluginInterface *>(p);
	if (plugin)
	{
		if(plugin->Init() == false)
		{
			delete plugin;
			loader->unload();
			delete loader;

			status.append(QString("%1 OK, Initialisation failed").arg(plugin->GetName()));
			LogInfo(status);
			return false;
		}

		listOfPlugins.append(plugin);
		listOfPluginsFileName.insert(plugin, fileName);
		listOfPluginsLoader.insert(plugin, loader);
		listOfPluginsByName.insert(plugin->GetName(), plugin);
		listOfPluginsByFileName.insert(fileName, plugin);
		if(plugin->GetType() != PluginInterface::BunnyPlugin && plugin->GetType() != PluginInterface::BunnyZtampPlugin && plugin->GetType() != PluginInterface::ZtampPlugin )
			listOfSystemPlugins.append(plugin);
		else
			BunnyManager::PluginLoaded(plugin);

		// Init Api Calls
		plugin->InitApiCalls();

		status.append(QString("%1 OK, Enable : %2").arg(plugin->GetName(),plugin->GetEnable() ? "Yes" : "No"));
		LogInfo(status);
		return true;
	}
	status.append("Failed, ").append(loader->errorString());
	LogInfo(status);
	return false;
}

bool PluginManager::UnloadPlugin(QString const& name)
{
	if(listOfPluginsByName.contains(name))
	{
		PluginInterface * p = listOfPluginsByName.value(name);
		if(p->GetType() == PluginInterface::BunnyPlugin || p->GetType() == PluginInterface::BunnyZtampPlugin)
			BunnyManager::PluginUnloaded(p);
		if(p->GetType() == PluginInterface::ZtampPlugin || p->GetType() == PluginInterface::BunnyZtampPlugin)
			ZtampManager::PluginUnloaded(p);
		QString fileName = listOfPluginsFileName.value(p);
		QPluginLoader * loader = listOfPluginsLoader.value(p);
		listOfPluginsByFileName.remove(fileName);
		listOfPluginsFileName.remove(p);
		listOfPluginsLoader.remove(p);
		listOfPluginsByName.remove(name);
		listOfPlugins.removeAll(p);
		listOfSystemPlugins.removeAll(p);
		delete p;
		loader->unload();
		delete loader;
		LogInfo(QString("Plugin %1 unloaded.").arg(name));
		return true;
	}
	LogInfo(QString("Can't unload plugin %1").arg(name));
	return false;
}

bool PluginManager::ReloadPlugin(QString const& name)
{
	if(listOfPluginsByName.contains(name))
	{
		PluginInterface * p = listOfPluginsByName.value(name);
		QString file = listOfPluginsFileName.value(p);
		return (UnloadPlugin(name) && LoadPlugin(file));
	}
	return false;
}

/**************************************************/
/* HTTP requests are sent to ALL 'active' plugins */
/**************************************************/
void PluginManager::HttpRequestBefore(HTTPRequest & request)
{
	// Call RequestBefore for all plugins
	foreach(PluginInterface * plugin, listOfPlugins)
		if(plugin->GetEnable())
			plugin->HttpRequestBefore(request);
}

bool PluginManager::HttpRequestHandle(HTTPRequest & request)
{
	// Call GetAnswer for all plugins until one returns true
	foreach(PluginInterface * plugin, listOfPlugins)
	{
		if(plugin->GetEnable() && plugin->HttpRequestHandle(request))
			return true;
	}
	return false;
}

void PluginManager::HttpRequestAfter(HTTPRequest & request)
{
	// Call RequestAfter for all plugins
	foreach(PluginInterface * plugin, listOfPlugins)
		if(plugin->GetEnable())
			plugin->HttpRequestAfter(request);
}

/*****************************************************/
/* Others requests are sent only to "system" plugins */
/*****************************************************/
// Bunny -> Violet Message
void PluginManager::XmppBunnyMessage(Bunny * b, QByteArray const& data)
{
	foreach(PluginInterface * plugin, listOfSystemPlugins)
		if(plugin->GetEnable())
			plugin->XmppBunnyMessage(b, data);
}

// Violet -> Bunny Message
void PluginManager::XmppVioletMessage(Bunny * b, QByteArray const& data)
{
	foreach(PluginInterface * plugin, listOfSystemPlugins)
		if(plugin->GetEnable())
			plugin->XmppVioletMessage(b, data);
}

// Violet -> Bunny Packet
// Send the packet to all 'system' plugins, if one returns true, the message will be dropped !
bool PluginManager::XmppVioletPacketMessage(Bunny * b, Packet const& p)
{
	bool drop = false;
	foreach(PluginInterface * plugin, listOfSystemPlugins)
		if(plugin->GetEnable())
			drop |= plugin->XmppVioletPacketMessage(b, p);
	return drop;
}

// Bunny OnClick
bool PluginManager::OnClick(Bunny * b, PluginInterface::ClickType type)
{
	// Call OnClick for all 'system' plugins until one returns true
	foreach(PluginInterface * plugin, listOfSystemPlugins)
	{
		if(plugin->GetEnable())
		{
			if(plugin->OnClick(b, type))
				return true;
		}
	}
	return false;
}

bool PluginManager::OnEarsMove(Bunny * b, int left, int right)
{
	// Call OnEarsMove for all 'system' plugins until one returns true
	foreach(PluginInterface * plugin, listOfSystemPlugins)
	{
		if(plugin->GetEnable())
		{
			if(plugin->OnEarsMove(b, left, right))
				return true;
		}
	}
	return false;
}

bool PluginManager::OnRFID(Ztamp * z, Bunny * b)
{
	// Call OnRFID for all 'system' plugins until one returns true
	foreach(PluginInterface * plugin, listOfSystemPlugins)
	{
		if(plugin->GetEnable())
		{
			if(plugin->OnRFID(z, b))
				return true;
		}
	}
	return false;
}

bool PluginManager::OnRFID(Bunny * b, QByteArray const& id)
{
	// Call OnRFID for all 'system' plugins until one returns true
	foreach(PluginInterface * plugin, listOfSystemPlugins)
	{
		if(plugin->GetEnable())
		{
			if(plugin->OnRFID(b, id))
				return true;
		}
	}
	return false;
}

// Bunny Connect
void PluginManager::OnBunnyConnect(Bunny * b)
{
	foreach(PluginInterface * plugin, listOfSystemPlugins)
		if(plugin->GetEnable())
			plugin->OnBunnyConnect(b);
}

// Bunny Connect
void PluginManager::OnBunnyDisconnect(Bunny * b)
{
	foreach(PluginInterface * plugin, listOfSystemPlugins)
		if(plugin->GetEnable())
			plugin->OnBunnyDisconnect(b);
}

// Ztamp Connect
void PluginManager::OnZtampConnect(Ztamp * b)
{
	foreach(PluginInterface * plugin, listOfSystemPlugins)
		if(plugin->GetEnable())
			plugin->OnZtampConnect(b);
}

// Ztamp Disconnect
void PluginManager::OnZtampDisconnect(Ztamp * b)
{
	foreach(PluginInterface * plugin, listOfSystemPlugins)
		if(plugin->GetEnable())
			plugin->OnZtampDisconnect(b);
}

/*******
 * API *
 *******/

void PluginManager::InitApiCalls()
{
	DECLARE_API_CALL("getListOfPlugins()", &PluginManager::Api_GetListOfPlugins);
	DECLARE_API_CALL("getListOfEnabledPlugins()", &PluginManager::Api_GetListOfEnabledPlugins);

	DECLARE_API_CALL("getListOfBunnyPlugins()", &PluginManager::Api_GetListOfBunnyPlugins);
	DECLARE_API_CALL("getListOfBunnyEnabledPlugins()", &PluginManager::Api_GetListOfBunnyEnabledPlugins);

	DECLARE_API_CALL("getListOfZtampPlugins()", &PluginManager::Api_GetListOfZtampPlugins);
	DECLARE_API_CALL("getListOfZtampEnabledPlugins()", &PluginManager::Api_GetListOfZtampEnabledPlugins);

	DECLARE_API_CALL("getListOfRequiredPlugins()", &PluginManager::Api_GetListOfRequiredPlugins);
	DECLARE_API_CALL("getListOfSystemPlugins()", &PluginManager::Api_GetListOfSystemPlugins);
	DECLARE_API_CALL("getListOfSystemEnabledPlugins()", &PluginManager::Api_GetListOfSystemEnabledPlugins);

	DECLARE_API_CALL("activatePlugin(name)", &PluginManager::Api_ActivatePlugin);
	DECLARE_API_CALL("deactivatePlugin(name)", &PluginManager::Api_DeactivatePlugin);
	DECLARE_API_CALL("loadPlugin(filename)", &PluginManager::Api_LoadPlugin);
	DECLARE_API_CALL("unloadPlugin(name)", &PluginManager::Api_UnloadPlugin);
	DECLARE_API_CALL("reloadPlugin(name)", &PluginManager::Api_ReloadPlugin);
}

API_CALL(PluginManager::Api_GetListOfPlugins)
{
	Q_UNUSED(hRequest);

	if(!account.HasAccess(Account::AcPlugins,Account::Read))
		return new ApiManager::ApiError("Access denied");

	QMap<QString, QVariant> list;
	foreach (PluginInterface * p, listOfPlugins)
		list.insert(p->GetName(), p->GetVisualName());

	return new ApiManager::ApiMappedList(list);
}

API_CALL(PluginManager::Api_GetListOfEnabledPlugins)
{
	Q_UNUSED(hRequest);

	if(!account.HasAccess(Account::AcPlugins,Account::Read))
		return new ApiManager::ApiError("Access denied");

	QList<QString> list;
	foreach (PluginInterface * p, listOfPlugins)
		if(p->GetEnable())
			list.append(p->GetName());

	return new ApiManager::ApiList(list);
}

API_CALL(PluginManager::Api_GetListOfBunnyPlugins)
{
	Q_UNUSED(hRequest);

	if(!account.HasAccess(Account::AcPluginsBunny,Account::Read))
		return new ApiManager::ApiError("Access denied");

	QList<QString> list;
	foreach (PluginInterface * p, listOfPlugins)
		if(p->GetType() == PluginInterface::BunnyPlugin || p->GetType() == PluginInterface::BunnyZtampPlugin)
			list.append(p->GetName());

	return new ApiManager::ApiList(list);
}

API_CALL(PluginManager::Api_GetListOfZtampPlugins)
{
	Q_UNUSED(hRequest);

	if(!account.HasAccess(Account::AcPluginsZtamp,Account::Read))
		return new ApiManager::ApiError("Access denied");

	QList<QString> list;
	foreach (PluginInterface * p, listOfPlugins)
		if(p->GetType() == PluginInterface::ZtampPlugin || p->GetType() == PluginInterface::BunnyZtampPlugin)
			list.append(p->GetName());

	return new ApiManager::ApiList(list);
}

API_CALL(PluginManager::Api_GetListOfSystemPlugins)
{
	Q_UNUSED(hRequest);

	if(!account.HasAccess(Account::AcServer,Account::Read))
		return new ApiManager::ApiError("Access denied");

	QList<QString> list;
	foreach (PluginInterface * p, listOfSystemPlugins)
		if(p->GetType() == PluginInterface::SystemPlugin)
			list.append(p->GetName());

	return new ApiManager::ApiList(list);
}

API_CALL(PluginManager::Api_GetListOfSystemEnabledPlugins)
{
	Q_UNUSED(hRequest);

	if(!account.HasAccess(Account::AcServer,Account::Read))
		return new ApiManager::ApiError("Access denied");

	QList<QString> list;
	foreach (PluginInterface * p, listOfSystemPlugins)
		if(p->GetType() == PluginInterface::SystemPlugin && p->GetEnable())
			list.append(p->GetName());

	return new ApiManager::ApiList(list);
}

API_CALL(PluginManager::Api_GetListOfRequiredPlugins)
{
	Q_UNUSED(hRequest);

	if(!account.HasAccess(Account::AcServer,Account::Read))
		return new ApiManager::ApiError("Access denied");

	QList<QString> list;
	foreach (PluginInterface * p, listOfPlugins)
		if(p->GetType() == PluginInterface::RequiredPlugin)
			list.append(p->GetName());

	return new ApiManager::ApiList(list);
}

API_CALL(PluginManager::Api_GetListOfBunnyEnabledPlugins)
{
	Q_UNUSED(hRequest);

	if(!account.HasAccess(Account::AcPluginsBunny,Account::Read))
		return new ApiManager::ApiError("Access denied");

	QList<QString> list;
	foreach (PluginInterface * p, listOfPlugins)
		if((p->GetType() == PluginInterface::BunnyPlugin || p->GetType() == PluginInterface::BunnyZtampPlugin) && p->GetEnable())
			list.append(p->GetName());

	return new ApiManager::ApiList(list);
}

API_CALL(PluginManager::Api_GetListOfZtampEnabledPlugins)
{
	Q_UNUSED(hRequest);

	if(!account.HasAccess(Account::AcPluginsZtamp,Account::Read))
		return new ApiManager::ApiError("Access denied");

	QList<QString> list;
	foreach (PluginInterface * p, listOfPlugins)
		if((p->GetType() == PluginInterface::ZtampPlugin || p->GetType() == PluginInterface::BunnyZtampPlugin) && p->GetEnable())
			list.append(p->GetName());

	return new ApiManager::ApiList(list);
}

API_CALL(PluginManager::Api_ActivatePlugin)
{
	Q_UNUSED(hRequest);

	if(!account.HasAccess(Account::AcServer,Account::Write))
		return new ApiManager::ApiError("Access denied");

	PluginInterface * p = listOfPluginsByName.value(hRequest.GetArg("name"));
	if(!p)
		return new ApiManager::ApiError(QString("Unknown plugin '%1'<br />Request was : %2").arg(hRequest.GetArg("name"),hRequest.toString()));

	if(p->GetEnable())
		return new ApiManager::ApiError(QString("Plugin '%1' is already enabled!").arg(hRequest.GetArg("name")));

	p->SetEnable(true);
	return new ApiManager::ApiOk(QString("'%1' is now enabled").arg(p->GetName()));
}

API_CALL(PluginManager::Api_DeactivatePlugin)
{
	Q_UNUSED(hRequest);

	if(!account.HasAccess(Account::AcServer,Account::Write))
		return new ApiManager::ApiError("Access denied");

	PluginInterface * p = listOfPluginsByName.value(hRequest.GetArg("name"));
	if(!p)
		return new ApiManager::ApiError(QString("Unknown plugin '%1'<br />Request was : %2").arg(hRequest.GetArg("name"),hRequest.toString()));

	if(p->GetType() == PluginInterface::RequiredPlugin)
		return new ApiManager::ApiError(QString("Plugin '%1' can't be deactivated!").arg(hRequest.GetArg("name")));

	if(!p->GetEnable())
		return new ApiManager::ApiError(QString("Plugin '%1' is already disabled!").arg(hRequest.GetArg("name")));

	p->SetEnable(false);
	return new ApiManager::ApiOk(QString("'%1' is now disabled").arg(p->GetName()));
}

API_CALL(PluginManager::Api_UnloadPlugin)
{
	Q_UNUSED(hRequest);

	if(!account.HasAccess(Account::AcServer,Account::Write))
		return new ApiManager::ApiError("Access denied");

	QString name = hRequest.GetArg("name");
	if(UnloadPlugin(name))
	{
		return new ApiManager::ApiOk(QString("'%1' is now unloaded").arg(name));
	}
	else
		return new ApiManager::ApiError(QString("Can't unload '%1'!").arg(name));
}

API_CALL(PluginManager::Api_LoadPlugin)
{
	Q_UNUSED(hRequest);

	if(!account.HasAccess(Account::AcServer,Account::Write))
		return new ApiManager::ApiError("Access denied");

	QString filename = hRequest.GetArg("filename");
	if(LoadPlugin(filename))
		return new ApiManager::ApiOk(QString("'%1' is now loaded").arg(filename));
	else
		return new ApiManager::ApiError(QString("Can't load '%1'!").arg(filename));
}

API_CALL(PluginManager::Api_ReloadPlugin)
{
	Q_UNUSED(hRequest);

	if(!account.HasAccess(Account::AcServer,Account::Write))
		return new ApiManager::ApiError("Access denied");

	QString name = hRequest.GetArg("name");
	if(ReloadPlugin(name))
		return new ApiManager::ApiOk(QString("'%1' is now reloaded").arg(name));
	else
		return new ApiManager::ApiError(QString("Can't reload '%1'!").arg(name));
}

/********************
 * Required Plugins *
 ********************/

void PluginManager::RegisterAuthPlugin(PluginAuthInterface * p)
{
	if(!authPlugin)
		authPlugin = p;
	else
		LogWarning("An authentication plugin is already registered.");
}

void PluginManager::UnregisterAuthPlugin(PluginAuthInterface * p)
{
	if(authPlugin == p)
		authPlugin = 0;
	else
		LogWarning("Bad plugin during UnregisterAuthPlugin");
}
