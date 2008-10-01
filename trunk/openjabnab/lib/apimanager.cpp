#include <QUrl>
#include <QStringList>

#include "accountmanager.h"
#include "apimanager.h"
#include "bunny.h"
#include "bunnymanager.h"
#include "httprequest.h"
#include "plugininterface.h"
#include "pluginmanager.h"

ApiManager::ApiManager()
{
}

ApiManager & ApiManager::Instance()
{
  static ApiManager a;
  return a;
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
	
	if (request.startsWith("plugins/"))
		return PluginManager::Instance().ProcessApiCall(request.mid(8), hRequest);

	if (request.startsWith("plugin/"))
		return ProcessPluginApiCall(request.mid(7), hRequest);

	if (request.startsWith("bunnies/"))
		return BunnyManager::ProcessApiCall(request.mid(8), hRequest);

	if (request.startsWith("bunny/"))
		return ProcessBunnyApiCall(request.mid(6), hRequest);
	
	if (request.startsWith("accounts/"))
		return AccountManager::Instance().ProcessApiCall(request.mid(9), hRequest);
	
	return new ApiManager::ApiError("Unknown Api Call : " + hRequest.toString());
}

ApiManager::ApiAnswer * ApiManager::ProcessGlobalApiCall(QByteArray const& /*request*/, HTTPRequest const& hRequest)
{
	return new ApiManager::ApiError("Unknown Global Api Call : " + hRequest.toString());
}

ApiManager::ApiAnswer * ApiManager::ProcessPluginApiCall(QByteArray const& request, HTTPRequest const& hRequest)
{
	QStringList list = QString(request).split('/', QString::SkipEmptyParts);
	
	if (list.size() != 2)
		return new ApiManager::ApiError("Malformed Plugin Api Call : " + hRequest.toString());
		
	QString const& pluginName = list.at(0);
	QString const& functionName = list.at(1);
	
	PluginInterface * plugin = PluginManager::Instance().GetPluginByName(pluginName);
	if(!plugin)
		return new ApiManager::ApiError("Unknown Plugin : " + pluginName.toUtf8() + "<br />" + "Request was : " + hRequest.toString());

	return plugin->ProcessApiCall(functionName.toAscii(), hRequest); 
}

ApiManager::ApiAnswer * ApiManager::ProcessBunnyApiCall(QByteArray const& request, HTTPRequest const& hRequest)
{
	QStringList list = QString(request).split('/', QString::SkipEmptyParts);
	
	if (list.size() != 2)
		return new ApiManager::ApiError("Malformed Bunny Api Call : " + hRequest.toString());
		
	QString const& bunnyID = list.at(0);
	QString const& functionName = list.at(1);
	
	Bunny * b = BunnyManager::GetBunny(bunnyID.toAscii());
	return b->ProcessApiCall(functionName.toAscii(), hRequest);
}

QByteArray ApiManager::ApiAnswer::SanitizeXML(QByteArray const& msg)
{
	if (msg.contains('<') || msg.contains('>') || msg.contains('&'))
		return "<![CDATA[" + msg + "]]>";
	return msg;
}

