#include <QUrl>
#include <QStringList>

#include "apimanager.h"
#include "bunny.h"
#include "bunnymanager.h"
#include "plugininterface.h"
#include "stdio.h"

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

ApiManager::ApiAnswer * ApiManager::ProcessApiCall(QByteArray const& request)
{
	if (request.startsWith("global/"))
		return ProcessGlobalApiCall(request.mid(7));
	
	if (request.startsWith("plugin/"))
		return ProcessPluginApiCall(request.mid(7));

	if (request.startsWith("bunny/"))
		return ProcessPluginApiCall(request.mid(6));
	
	return new ApiManager::ApiError("Unknown Api Call : " + request);
}

ApiManager::ApiAnswer * ApiManager::ProcessGlobalApiCall(QByteArray const& request)
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

	return new ApiManager::ApiError("Unknown Global Api Call : " + request);
}

ApiManager::ApiAnswer * ApiManager::ProcessPluginApiCall(QByteArray const& request)
{
	QUrl url(request); // Used to handle ?param=x and percent encoding
	QStringList list = url.path().split('/', QString::SkipEmptyParts);
	
	if (list.size() != 2)
		return new ApiManager::ApiError("Malformed Plugin Api Call : " + url.path().toUtf8());
		
	QString const& pluginName = list.at(0);
	QString const& functionName = list.at(1);
	
	PluginInterface * plugin = pluginManager->GetPluginByName(pluginName);
	if(!plugin)
		return new ApiManager::ApiError("Unknown Plugin : " + pluginName.toUtf8());

	if(functionName == "enable" || functionName == "disable")
	{
		plugin->SetEnable(functionName == "enable" ? true : false);
		return new ApiManager::ApiString(plugin->GetName().toUtf8() + " is now "+ ( functionName == "enable" ? "enabled" : "disabled") +".");
	}
	else
	{
		return new ApiManager::ApiError("Unknown Plugin Api Call : " + functionName.toUtf8());
	}
}

ApiManager::ApiAnswer * ApiManager::ProcessBunnyApiCall(QByteArray const& request)
{
	return new ApiManager::ApiError("Unknown Bunny Api Call : " + request);
}

QByteArray ApiManager::ApiAnswer::SanitizeXML(QByteArray const& msg)
{
	if (msg.contains('<') || msg.contains('>') || msg.contains('&'))
		return "<![CDATA[" + msg + "]]>";
	return msg;
}

