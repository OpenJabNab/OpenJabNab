#include <QCoreApplication>
#include <QDir>
#include <QLibrary>
#include <QPluginLoader>
#include <QString>
#include "apimanager.h"
#include "httprequest.h"
#include "log.h"
#include "pluginmanager.h"

PluginManager::PluginManager()
{
	// Load all plugins
	QDir pluginsDir = QCoreApplication::applicationDirPath();
	pluginsDir.cd("plugins");

	Log::Info(QString("Finding plugins in : %1").arg(pluginsDir.path()));
	
	foreach (QString fileName, pluginsDir.entryList(QDir::Files)) 
	{
		QString file = pluginsDir.absoluteFilePath(fileName);
		if (!QLibrary::isLibrary(file))
			continue;

		QString status = QString(" - %1 : ").arg(fileName);
		
		QPluginLoader loader(file);
		QObject * p = loader.instance();
		PluginInterface * plugin = qobject_cast<PluginInterface *>(p);
		if (plugin)
		{
			listOfPlugins.append(plugin);
			listOfPluginsByName.insert(plugin->GetName(), plugin);
			status.append(plugin->GetName() + " OK, Enable : " + ( plugin->GetEnable() ? "Yes" : "No" ) );
		}
		else
			status.append("Failed, ").append(loader.errorString()); 
		Log::Info(status);
	}
}

PluginManager::~PluginManager()
{
	foreach(PluginInterface * p, listOfPlugins)
		delete p;
}

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
		if(plugin->HttpRequestHandle(request))
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
	
void PluginManager::XmppBunnyMessage(QByteArray const& data)
{
	foreach(PluginInterface * plugin, listOfPlugins)
		if(plugin->GetEnable())
			plugin->XmppBunnyMessage(data);
}

void PluginManager::XmppVioletMessage(QByteArray const& data)
{
	foreach(PluginInterface * plugin, listOfPlugins)
		if(plugin->GetEnable())
			plugin->XmppVioletMessage(data);
}

// Send the packet to all plugins, if one returns true, the message will be dropped !
bool PluginManager::XmppVioletPacketMessage(Packet const& p)
{
	bool drop = false;
	foreach(PluginInterface * plugin, listOfPlugins)
		if(plugin->GetEnable())
			drop |= plugin->XmppVioletPacketMessage(p);
	return drop;
}

bool PluginManager::OnClick(Bunny * b, PluginInterface::ClickType type)
{
	// Call OnClick for all plugins until one returns true
	foreach(PluginInterface * plugin, listOfPlugins)
	{
		if(plugin->HasBunny(b))
		{
			if(plugin->OnClick(b, type))
				return true;
		}
	}
	return false;
}

bool PluginManager::OnEarsMove(Bunny * b, int left, int right)
{
	// Call OnClick for all plugins until one returns true
	foreach(PluginInterface * plugin, listOfPlugins)
	{
		if(plugin->HasBunny(b))
		{
			if(plugin->OnEarsMove(b, left, right))
				return true;
		}
	}
	return false;
}

bool PluginManager::OnRFID(Bunny * b, QByteArray const& id)
{
	// Call OnClick for all plugins until one returns true
	foreach(PluginInterface * plugin, listOfPlugins)
	{
		if(plugin->HasBunny(b))
		{
			if(plugin->OnRFID(b, id))
				return true;
		}
	}
	return false;
}

PluginManager * PluginManager::Instance() {
  static PluginManager p;
  return &p;
}

ApiManager::ApiAnswer * PluginManager::ProcessApiCall(QByteArray const& request, HTTPRequest const& hRequest)
{
	if(request.startsWith("getListOfPlugins"))
	{
		QList<QByteArray> list;
		foreach (PluginInterface * p, listOfPlugins)
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
	else
		return new ApiManager::ApiError("Unknown Plugins Api Call : " + request + "<br />Request was : " + hRequest.toString());
}
