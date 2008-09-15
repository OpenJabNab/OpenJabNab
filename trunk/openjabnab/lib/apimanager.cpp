#include <QUrl>
#include <QStringList>

#include "apimanager.h"
#include "bunny.h"
#include "bunnymanager.h"
#include "httprequest.h"
#include "plugininterface.h"

ApiManager::ApiManager(PluginManager * p):pluginManager(p)
{
}

QByteArray ApiManager::ApiAnswer::GetData()
{
	QByteArray tmp("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	tmp.append("<api>");
	tmp.append(GetInternalData());
	tmp.append("</api>");
	return tmp;
}

ApiManager::ApiAnswer * ApiManager::ProcessApiCall(QByteArray const& request, HTTPRequest const& hRequest)
{
	if (request.startsWith("global/"))
		return ProcessGlobalApiCall(request.mid(7), hRequest);
	
	if (request.startsWith("plugin/"))
		return ProcessPluginApiCall(request.mid(7), hRequest);

	if (request.startsWith("bunny/"))
		return ProcessBunnyApiCall(request.mid(6), hRequest);
	
	return new ApiManager::ApiError("Unknown Api Call : " + hRequest.toString());
}

ApiManager::ApiAnswer * ApiManager::ProcessGlobalApiCall(QByteArray const& request, HTTPRequest const& hRequest)
{
	if (request.startsWith("getListOfConnectedBunnies"))
	{
		QList<QByteArray> listOfBunnies;
		foreach (Bunny * b, BunnyManager::GetConnectedBunnies())
			listOfBunnies.append(b->GetID());

		return new ApiManager::ApiList(listOfBunnies);
	}
	
	if (request.startsWith("getListOfPlugins"))
	{
		QList<QByteArray> listOfPlugins;
		foreach (PluginInterface * p, pluginManager->GetListOfPlugins())
			listOfPlugins.append(p->GetName().toAscii());

		return new ApiManager::ApiList(listOfPlugins);
	}

	if (request.startsWith("GetListOfBunnyPlugins"))
	{
		QList<QByteArray> listOfPlugins;
		foreach (PluginInterface * p, pluginManager->GetListOfPlugins())
			if(p->GetType() == PluginInterface::BunnyPlugin)
				listOfPlugins.append(p->GetName().toAscii());

		return new ApiManager::ApiList(listOfPlugins);
	}

	if (request.startsWith("GetListOfSystemPlugins"))
	{
		QList<QByteArray> listOfPlugins;
		foreach (PluginInterface * p, pluginManager->GetListOfPlugins())
			if(p->GetType() == PluginInterface::SystemPlugin)
				listOfPlugins.append(p->GetName().toAscii());

		return new ApiManager::ApiList(listOfPlugins);
	}

	if (request.startsWith("GetListOfRequiredPlugins"))
	{
		QList<QByteArray> listOfPlugins;
		foreach (PluginInterface * p, pluginManager->GetListOfPlugins())
			if(p->GetType() == PluginInterface::RequiredPlugin)
				listOfPlugins.append(p->GetName().toAscii());

		return new ApiManager::ApiList(listOfPlugins);
	}

	return new ApiManager::ApiError("Unknown Global Api Call : " + hRequest.toString());
}

ApiManager::ApiAnswer * ApiManager::ProcessPluginApiCall(QByteArray const& request, HTTPRequest const& hRequest)
{
	QStringList list = QString(request).split('/', QString::SkipEmptyParts);
	
	if (list.size() != 2)
		return new ApiManager::ApiError("Malformed Plugin Api Call : " + hRequest.toString());
		
	QString const& pluginName = list.at(0);
	QString const& functionName = list.at(1);
	
	PluginInterface * plugin = pluginManager->GetPluginByName(pluginName);
	if(!plugin)
		return new ApiManager::ApiError("Unknown Plugin : " + pluginName.toUtf8() + "<br />" + "Request was : " + hRequest.toString());

	if(functionName == "ActivatePlugin" || functionName == "DeactivatePlugin")
	{
		plugin->SetEnable(functionName == "ActivatePlugin" ? true : false);
		return new ApiManager::ApiString(plugin->GetName().toUtf8() + " is now "+ ( functionName == "ActivatePlugin" ? "enabled" : "disabled") +".");
	}
	else
	{
		return new ApiManager::ApiError("Unknown Plugin Api Call : " + functionName.toUtf8() + "<br />" + "Request was : " + hRequest.toString());
	}
}

ApiManager::ApiAnswer * ApiManager::ProcessBunnyApiCall(QByteArray const& request, HTTPRequest const& hRequest)
{
	QStringList list = QString(request).split('/', QString::SkipEmptyParts);
	
	if (list.size() != 2)
		return new ApiManager::ApiError("Malformed Bunny Api Call : " + hRequest.toString());
		
	QString const& bunnyID = list.at(0);
	QString const& functionName = list.at(1);
	
	Bunny * b = BunnyManager::GetBunny(bunnyID.toAscii());
	if(functionName == "RegisterPlugin")
	{
		if(hRequest.HasArg("name"))
		{
			PluginInterface * plugin = pluginManager->GetPluginByName(hRequest.GetArg("name"));
			plugin->RegisterBunny(b);
			return new ApiManager::ApiString("Bunny "+ b->GetID() +" is registering plugin " + plugin->GetVisualName() + ".");
		}
		return new ApiManager::ApiError("Missing argument in Bunny Api Call 'RegisterPlugin' : " + hRequest.toString());
	}
	else if(functionName == "UnregisterPlugin")
	{
		if(hRequest.HasArg("name"))
		{
			PluginInterface * plugin = pluginManager->GetPluginByName(hRequest.GetArg("name"));
			plugin->UnregisterBunny(b);
			return new ApiManager::ApiString("Bunny "+ b->GetID() +" is unregistering plugin " + plugin->GetVisualName() + ".");
		}
		return new ApiManager::ApiError("Missing argument in Bunny Api Call 'UnregisterPlugin' : " + hRequest.toString());
	}
	else
	{
		return new ApiManager::ApiError("Unknown Bunny Api Call : " + hRequest.toString());
	}
}

QByteArray ApiManager::ApiAnswer::SanitizeXML(QByteArray const& msg)
{
	if (msg.contains('<') || msg.contains('>') || msg.contains('&'))
		return "<![CDATA[" + msg + "]]>";
	return msg;
}

