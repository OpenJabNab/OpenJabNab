#include <QCoreApplication>
#include <QDir>
#include <QLibrary>
#include <QPluginLoader>
#include <QString>
#include "apimanager.h"
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

PluginManager & PluginManager::Instance() {
  static PluginManager p;
  return p;
}

void PluginManager::Close()
{
	foreach(PluginInterface * p, listOfPlugins)
		delete p;
	foreach(QPluginLoader * l, listOfPluginsLoader.values())
	{
		l->unload();
		delete l;
	}
}

void PluginManager::LoadPlugins()
{
	Log::Info(QString("Finding plugins in : %1").arg(pluginsDir.path()));
	foreach (QString fileName, pluginsDir.entryList(QDir::Files)) 
	{
		std::cout << "LOADING : " << qPrintable(fileName) << std::endl;
		LoadPlugin(fileName);
		std::cout << "END LOADING : " << qPrintable(fileName) << std::endl;
	}
}

bool PluginManager::LoadPlugin(QString const& fileName)
{
	if(listOfPluginsByFileName.contains(fileName))
	{
		Log::Error("Plugin " + fileName + " already loaded !");
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
		listOfPlugins.append(plugin);
		listOfPluginsFileName.insert(plugin, fileName);
		listOfPluginsLoader.insert(plugin, loader);
		listOfPluginsByName.insert(plugin->GetName(), plugin);
		listOfPluginsByFileName.insert(fileName, plugin);
		if(plugin->GetType() != PluginInterface::BunnyPlugin)
			listOfSystemPlugins.append(plugin);
		status.append(plugin->GetName() + " OK, Enable : " + ( plugin->GetEnable() ? "Yes" : "No" ) );
		Log::Info(status);
		return true;
	}
	status.append("Failed, ").append(loader->errorString()); 
	Log::Info(status);
	return false;
}

bool PluginManager::UnloadPlugin(QString const& name)
{
	if(listOfPluginsByName.contains(name))
	{
		PluginInterface * p = listOfPluginsByName.value(name);
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
		Log::Info(QString("Plugin %1 unloaded.").arg(name));
		return true;
	}
	Log::Info(QString("Can't unload plugin %1").arg(name));
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
void PluginManager::HttpRequestBefore(HTTPRequest const& request)
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

void PluginManager::HttpRequestAfter(HTTPRequest const& request)
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

/*******/
/* API */
/*******/
ApiManager::ApiAnswer * PluginManager::ProcessApiCall(QByteArray const& request, HTTPRequest const& hRequest)
{
	if(request.startsWith("getListOfPlugins"))
	{
		QMap<QByteArray, QByteArray> list;
		foreach (PluginInterface * p, listOfPlugins)
			list.insert(p->GetName().toAscii(), p->GetVisualName().toAscii());
		return new ApiManager::ApiMappedList(list);
	}
	else if(request.startsWith("getListOfEnabledPlugins"))
	{
		QList<QByteArray> list;
		foreach (PluginInterface * p, listOfPlugins)
			if(p->GetEnable())
				list.append(p->GetName().toAscii());

		return new ApiManager::ApiList(list);
	}
	else if(request.startsWith("getListOfBunnyPlugins"))
	{
		QList<QByteArray> list;
		foreach (PluginInterface * p, listOfPlugins)
			if(p->GetType() == PluginInterface::BunnyPlugin)
				list.append(p->GetName().toAscii());

		return new ApiManager::ApiList(list);
	}
	else if(request.startsWith("getListOfSystemPlugins"))
	{
		QList<QByteArray> list;
		foreach (PluginInterface * p, listOfPlugins)
			if(p->GetType() == PluginInterface::SystemPlugin)
				list.append(p->GetName().toAscii());

		return new ApiManager::ApiList(list);
	}
	else if(request.startsWith("getListOfRequiredPlugins"))
	{
		QList<QByteArray> list;
		foreach (PluginInterface * p, listOfPlugins)
			if(p->GetType() == PluginInterface::RequiredPlugin)
				list.append(p->GetName().toAscii());

		return new ApiManager::ApiList(list);
	}
	else if(request == "activatePlugin")
	{
		if(!hRequest.HasArg("name"))
			return new ApiManager::ApiError("Missing 'name' argument<br />Request was : " + hRequest.toString());
		PluginInterface * p = listOfPluginsByName.value(hRequest.GetArg("name"));
		if(!p)
			return new ApiManager::ApiError("Unknown plugin : " + hRequest.GetArg("name") + "<br />Request was : " + hRequest.toString());
		p->SetEnable(true);
		return new ApiManager::ApiString(p->GetName() + " is now enabled.");
	}
	else if(request == "deactivatePlugin")
	{
		if(!hRequest.HasArg("name"))
			return new ApiManager::ApiError("Missing 'name' argument<br />Request was : " + hRequest.toString());
		PluginInterface * p = listOfPluginsByName.value(hRequest.GetArg("name"));
		if(!p)
			return new ApiManager::ApiError("Unknown plugin : " + hRequest.GetArg("name") + "<br />Request was : " + hRequest.toString());
		p->SetEnable(false);
		return new ApiManager::ApiString(p->GetName() + " is now disabled.");
	}
	else if(request == "unloadPlugin")
	{
		if(!hRequest.HasArg("name"))
			return new ApiManager::ApiError("Missing 'name' argument<br />Request was : " + hRequest.toString());
		QString name = hRequest.GetArg("name");
		if(UnloadPlugin(name))
			return new ApiManager::ApiOk(name + " is now unloaded.");
		else
			return new ApiManager::ApiError("Can't unload " + name);
	}
	else if(request == "loadPlugin")
	{
		if(!hRequest.HasArg("filename"))
			return new ApiManager::ApiError("Missing 'filename' argument<br />Request was : " + hRequest.toString());
		QString filename = hRequest.GetArg("filename");
		if(LoadPlugin(filename))
			return new ApiManager::ApiOk(filename + " is now loaded.");
		else
			return new ApiManager::ApiError("Can't load " + filename);
	}
	else if(request == "reloadPlugin")
	{
		if(!hRequest.HasArg("name"))
			return new ApiManager::ApiError("Missing 'name' argument<br />Request was : " + hRequest.toString());
		QString name = hRequest.GetArg("name");
		if(ReloadPlugin(name))
			return new ApiManager::ApiOk(name + " is now reloaded.");
		else
			return new ApiManager::ApiError("Can't reload " + name);
	}
	else
		return new ApiManager::ApiError("Unknown Plugins Api Call : " + request + "<br />Request was : " + hRequest.toString());
}
