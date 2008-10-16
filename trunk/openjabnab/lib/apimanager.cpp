#include <QUrl>
#include <QStringList>

#include "account.h"
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

ApiManager::ApiAnswer * ApiManager::ProcessApiCall(QByteArray const& request, HTTPRequest & hRequest)
{
	Account const& account = hRequest.HasArg("token")?AccountManager::Instance().GetAccount(hRequest.GetArg("token").toAscii()):AccountManager::Guest();
	hRequest.RemoveArg("token");

	if(request.startsWith("global/"))
		return ProcessGlobalApiCall(account, request.mid(7), hRequest);
	
	if(request.startsWith("plugins/"))
		return PluginManager::Instance().ProcessApiCall(account, request.mid(8), hRequest);

	if(request.startsWith("plugin/"))
		return ProcessPluginApiCall(account, request.mid(7), hRequest);

	if(request.startsWith("bunnies/"))
		return BunnyManager::ProcessApiCall(account, request.mid(8), hRequest);

	if(request.startsWith("bunny/"))
		return ProcessBunnyApiCall(account, request.mid(6), hRequest);
	
	if(request.startsWith("accounts/"))
		return AccountManager::Instance().ProcessApiCall(account, request.mid(9), hRequest);
	
	return new ApiManager::ApiError("Unknown Api Call : " + hRequest.toString());
}

ApiManager::ApiAnswer * ApiManager::ProcessGlobalApiCall(Account const& account, QByteArray const& request, HTTPRequest const& hRequest)
{
	if(!account.HasGlobalAccess(Account::Read))
			return new ApiManager::ApiError(QByteArray("Access denied"));

	if(request == "about")
	{
		return new ApiManager::ApiString(QByteArray("OpenJabNab v0.01 - (Build " __DATE__ " / " __TIME__ ")"));
	}
	return new ApiManager::ApiError("Unknown Global Api Call : " + hRequest.toString());
}

ApiManager::ApiAnswer * ApiManager::ProcessPluginApiCall(Account const& account, QByteArray const& request, HTTPRequest & hRequest)
{
	QStringList list = QString(request).split('/', QString::SkipEmptyParts);
	
	if(list.size() != 2)
		return new ApiManager::ApiError("Malformed Plugin Api Call : " + hRequest.toString());
		
	QString const& pluginName = list.at(0);
	QString const& functionName = list.at(1);

	PluginInterface * plugin = PluginManager::Instance().GetPluginByName(pluginName);
	if(!plugin)
		return new ApiManager::ApiError("Unknown Plugin : " + pluginName.toUtf8() + "<br />" + "Request was : " + hRequest.toString());

	if(hRequest.HasArg("bunny"))
	{
		QByteArray bunnyID = hRequest.GetArg("bunny").toAscii();
		if(account.HasBunnyAccess(bunnyID))
		{
			hRequest.RemoveArg("bunny");
			return plugin->ProcessBunnyApiCall(BunnyManager::GetBunny(bunnyID), account, functionName.toAscii(), hRequest);
		}
		else
			return new ApiManager::ApiError(QByteArray("Access denied to this bunny"));
	}
	return plugin->ProcessApiCall(account, functionName.toAscii(), hRequest);
}

ApiManager::ApiAnswer * ApiManager::ProcessBunnyApiCall(Account const& account, QByteArray const& request, HTTPRequest const& hRequest)
{
	QStringList list = QString(request).split('/', QString::SkipEmptyParts);
	
	if(list.size() != 2)
		return new ApiManager::ApiError("Malformed Bunny Api Call : " + hRequest.toString());
		
	QByteArray const& bunnyID = list.at(0).toAscii();
	QByteArray const& functionName = list.at(1).toAscii();
	
	if(!account.HasBunnyAccess(bunnyID))
		return new ApiManager::ApiError(QByteArray("Access denied to this bunny"));
	
	return BunnyManager::GetBunny(bunnyID)->ProcessApiCall(functionName, hRequest);
}

QByteArray ApiManager::ApiAnswer::SanitizeXML(QByteArray const& msg)
{
	if(msg.contains('<') || msg.contains('>') || msg.contains('&'))
		return "<![CDATA[" + msg + "]]>";
	return msg;
}

