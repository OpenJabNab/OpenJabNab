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
	QString tmp("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	tmp.append("<api>");
	tmp.append(GetInternalData());
	tmp.append("</api>");
	return tmp.toUtf8();
}

ApiManager::ApiAnswer * ApiManager::ProcessApiCall(QString const& request, HTTPRequest & hRequest)
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
		return BunnyManager::Instance().ProcessApiCall(account, request.mid(8), hRequest);

	if(request.startsWith("bunny/"))
		return ProcessBunnyApiCall(account, request.mid(6), hRequest);
	
	if(request.startsWith("accounts/"))
		return AccountManager::Instance().ProcessApiCall(account, request.mid(9), hRequest);
	
	return new ApiManager::ApiError(QString("Unknown Api Call : %1").arg(hRequest.toString()));
}

ApiManager::ApiAnswer * ApiManager::ProcessGlobalApiCall(Account const& account, QString const& request, HTTPRequest const& hRequest)
{
	if(!account.HasGlobalAccess(Account::Read))
			return new ApiManager::ApiError("Access denied");

	if(request == "about")
	{
		return new ApiManager::ApiString("OpenJabNab v0.01 - (Build " __DATE__ " / " __TIME__ ")");
	}
	return new ApiManager::ApiError(QString("Unknown Global Api Call : %1").arg(hRequest.toString()));
}

ApiManager::ApiAnswer * ApiManager::ProcessPluginApiCall(Account const& account, QString const& request, HTTPRequest & hRequest)
{
	QStringList list = QString(request).split('/', QString::SkipEmptyParts);
	
	if(list.size() != 2)
		return new ApiManager::ApiError(QString("Malformed Plugin Api Call : %1").arg(hRequest.toString()));
		
	QString const& pluginName = list.at(0);
	QString const& functionName = list.at(1);

	PluginInterface * plugin = PluginManager::Instance().GetPluginByName(pluginName);
	if(!plugin)
		return new ApiManager::ApiError(QString("Unknown Plugin : %1<br />Request was : %2").arg(pluginName,hRequest.toString()));

	if(!plugin->GetEnable())
		return new ApiManager::ApiError("This plugin is disabled");

	if(hRequest.HasArg("bunny"))
	{
		QByteArray bunnyID = hRequest.GetArg("bunny").toAscii();
		if(account.HasBunnyAccess(bunnyID))
		{
			hRequest.RemoveArg("bunny");
			Bunny * b = BunnyManager::GetBunny(bunnyID);
			if(b->HasPlugin(plugin))
					return plugin->ProcessBunnyApiCall(b, account, functionName, hRequest);
			else
				return new ApiManager::ApiError("This plugin is disabled for this bunny");
		}
		else
			return new ApiManager::ApiError("Access denied to this bunny");
	}
	return plugin->ProcessApiCall(account, functionName, hRequest);
}

ApiManager::ApiAnswer * ApiManager::ProcessBunnyApiCall(Account const& account, QString const& request, HTTPRequest const& hRequest)
{
	QStringList list = QString(request).split('/', QString::SkipEmptyParts);
	
	if(list.size() != 2)
		return new ApiManager::ApiError(QString("Malformed Bunny Api Call : %1").arg(hRequest.toString()));
		
	QByteArray const& bunnyID = list.at(0).toAscii();
	QString const& functionName = list.at(1);
	
	if(!account.HasBunnyAccess(bunnyID))
		return new ApiManager::ApiError("Access denied to this bunny");
	
	return BunnyManager::GetBunny(bunnyID)->ProcessApiCall(account, functionName, hRequest);
}

QString ApiManager::ApiAnswer::SanitizeXML(QString const& msg)
{
	if(msg.contains('<') || msg.contains('>') || msg.contains('&'))
		return "<![CDATA[" + msg + "]]>";
	return msg;
}

QString ApiManager::ApiError::GetInternalData()
{
	return QString("<error>%1</error>").arg(SanitizeXML(error));
}

QString ApiManager::ApiOk::GetInternalData()
{
	return QString("<ok>%1</ok>").arg(SanitizeXML(string));
}

QString ApiManager::ApiString::GetInternalData()
{
	return QString("<value>%1</value>").arg(SanitizeXML(string));
}

QString ApiManager::ApiList::GetInternalData()
{ 
	QString tmp;
	tmp += "<list>";
	foreach (QString b, list)
		tmp += QString("<item>%1</item>").arg(SanitizeXML(b));
	tmp += "</list>";
	return tmp;
}

QString ApiManager::ApiMappedList::GetInternalData()
{ 
	QString tmp;
	tmp += "<list>";
	QMapIterator<QString, QString> i(list);
	while (i.hasNext()) {
		i.next();
		tmp += QString("<item><key>%1</key><value>%2</value></item>").arg(SanitizeXML(i.key()), SanitizeXML(i.value()));
	}
	tmp += "</list>";
	return tmp;
}
