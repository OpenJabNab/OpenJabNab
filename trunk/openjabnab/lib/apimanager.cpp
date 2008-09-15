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
		return ProcessPluginApiCall(request.mid(6), hRequest);
	
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

	if(functionName == "enable" || functionName == "disable")
	{
		plugin->SetEnable(functionName == "enable" ? true : false);
		return new ApiManager::ApiString(plugin->GetName().toUtf8() + " is now "+ ( functionName == "enable" ? "enabled" : "disabled") +".");
	}
	else
	{
		return new ApiManager::ApiError("Unknown Plugin Api Call : " + functionName.toUtf8() + "<br />" + "Request was : " + hRequest.toString());
	}
}

ApiManager::ApiAnswer * ApiManager::ProcessBunnyApiCall(QByteArray const& /*request*/, HTTPRequest const& hRequest)
{
	return new ApiManager::ApiError("Unknown Bunny Api Call : " + hRequest.toString());
}

QByteArray ApiManager::ApiAnswer::SanitizeXML(QByteArray const& msg)
{
	if (msg.contains('<') || msg.contains('>') || msg.contains('&'))
		return "<![CDATA[" + msg + "]]>";
	return msg;
}

