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
	if(request.startsWith("/ojn/FR/api"))
	{
		return ProcessBunnyVioletApiCall(request, hRequest);
	}
	else
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

		if(request.startsWith("ztamps/"))
			return ZtampManager::Instance().ProcessApiCall(account, request.mid(7), hRequest);

		if(request.startsWith("ztamp/"))
			return ProcessZtampApiCall(account, request.mid(6), hRequest);

		if(request.startsWith("accounts/"))
			return AccountManager::Instance().ProcessApiCall(account, request.mid(9), hRequest);

		return new ApiManager::ApiError(QString("Unknown Api Call : %1").arg(hRequest.toString()));
	}
}

ApiManager::ApiAnswer * ApiManager::ProcessGlobalApiCall(Account const& account, QString const& request, HTTPRequest const& hRequest)
{
	if(request == "about")
	{
		return new ApiManager::ApiString("OpenJabNab v0.01 - (Build " __DATE__ " / " __TIME__ ")");
	}
	else if(request == "ping")
	{
		return new ApiManager::ApiString(QString::number(BunnyManager::Instance().GetConnectedBunnyCount()) + "/" + QString::number(GlobalSettings::GetInt("Config/MaxNumberOfBunnies", 64)) + "/" + QString::number(GlobalSettings::GetInt("Config/MaxBurstNumberOfBunnies", GlobalSettings::GetInt("Config/MaxNumberOfBunnies", 64))));
	}
	else if (request == "stats")
	{
		int bunnies = BunnyManager::Instance().GetBunnyCount();
		int connectedBunnies = BunnyManager::Instance().GetConnectedBunnyCount();

		int ztamps = ZtampManager::Instance().GetZtampCount();

		int plugins = PluginManager::Instance().GetPluginCount();
		int enabledPlugins = PluginManager::Instance().GetEnabledPluginCount();

		QString stats = "<bunnies>" + QString::number(bunnies) + "</bunnies>";
		stats += "<connected_bunnies>" + QString::number(connectedBunnies) + "</connected_bunnies>";
		stats += "<ztamps>" + QString::number(ztamps) + "</ztamps>";
		stats += "<plugins>" + QString::number(plugins) + "</plugins>";
		stats += "<enabled_plugins>" + QString::number(enabledPlugins) + "</enabled_plugins>";
		return new ApiManager::ApiXml(stats);
	}

	if(!account.HasAccess(Account::AcGlobal,Account::Read))
			return new ApiManager::ApiError("Access denied");

	if (request == "getListOfApiCalls")
	{
		// Todo send a list with available api calls
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

	return plugin->ProcessApiCall(account, functionName, hRequest);
}

ApiManager::ApiAnswer * ApiManager::ProcessBunnyApiCall(Account const& account, QString const& request, HTTPRequest const& hRequest)
{
	QStringList list = QString(request).split('/', QString::SkipEmptyParts);

	if(list.size() < 2)
		return new ApiManager::ApiError(QString("Malformed Bunny Api Call : %1").arg(hRequest.toString()));

	QByteArray const& bunnyID = list.at(0).toAscii();

	if(!account.HasBunnyAccess(bunnyID))
		return new ApiManager::ApiError("Access denied to this bunny");

	Bunny * b = BunnyManager::GetBunny(bunnyID);

	if(list.size() == 2)
	{
		QByteArray const& functionName = list.at(1).toAscii();
		return b->ProcessApiCall(account, functionName, hRequest);
	}
	else if(list.size() == 3)
	{
			PluginInterface * plugin = PluginManager::Instance().GetPluginByName(list.at(1).toAscii());
			if(!plugin)
				return new ApiManager::ApiError(QString("Unknown Plugin : '%1'").arg(list.at(1)));

			if(b->HasPlugin(plugin))
			{
				QByteArray const& functionName = list.at(2).toAscii();
				return plugin->ProcessBunnyApiCall(b, account, functionName, hRequest);
			}
		else
			return new ApiManager::ApiError("This plugin is not enabled for this bunny");
	}
	else
		return new ApiManager::ApiError(QString("Malformed Plugin Api Call : %1").arg(hRequest.toString()));
}

ApiManager::ApiAnswer * ApiManager::ProcessBunnyVioletApiCall(QString const& request, HTTPRequest const& hRequest)
{
	QStringList list = QString(request).split('/', QString::SkipEmptyParts);

	if(list.size() < 3)
		return new ApiManager::ApiError(QString("Malformed Bunny Api Call : %1").arg(hRequest.toString()));

	QString serial = hRequest.GetArg("sn");

	Bunny * b = BunnyManager::GetBunny(serial.toAscii());

	if(list.size() == 3)
	{
		return b->ProcessVioletApiCall(hRequest);
	}
	else
		return new ApiManager::ApiError(QString("Malformed Plugin Api Call : %1").arg(hRequest.toString()));
}

ApiManager::ApiAnswer * ApiManager::ProcessZtampApiCall(Account const& account, QString const& request, HTTPRequest const& hRequest)
{
	QStringList list = QString(request).split('/', QString::SkipEmptyParts);

	if(list.size() < 2)
		return new ApiManager::ApiError(QString("Malformed Ztamp Api Call : %1").arg(hRequest.toString()));

	QByteArray const& ztampID = list.at(0).toAscii();

	if(!account.HasZtampAccess(ztampID))
		return new ApiManager::ApiError("Access denied to this ztamp");

	Ztamp * z = ZtampManager::GetZtamp(ztampID);

	if(list.size() == 2)
	{
		QByteArray const& functionName = list.at(1).toAscii();
		return z->ProcessApiCall(account, functionName, hRequest);
	}
	else if(list.size() == 3)
	{
			PluginInterface * plugin = PluginManager::Instance().GetPluginByName(list.at(1).toAscii());
			if(!plugin)
				return new ApiManager::ApiError(QString("Unknown Plugin : '%1'").arg(list.at(1)));

			if(z->HasPlugin(plugin))
			{
				QByteArray const& functionName = list.at(2).toAscii();
				return plugin->ProcessZtampApiCall(z, account, functionName, hRequest);
			}
		else
			return new ApiManager::ApiError("This plugin is not enabled for this ztamp");
	}
	else
		return new ApiManager::ApiError(QString("Malformed Plugin Api Call : %1").arg(hRequest.toString()));
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
	QMapIterator<QString, QVariant> i(list);
	while (i.hasNext()) {
		i.next();
		tmp += QString("<item><key>%1</key><value>%2</value></item>").arg(SanitizeXML(i.key()), SanitizeXML(i.value().toString()));
	}
	tmp += "</list>";
	return tmp;
}

void ApiManager::ApiViolet::AddMessage(QString m, QString c)
{
	string += "<message>" + m + "</message>";
	string += "<comment>" + c + "</comment>";
}

void ApiManager::ApiViolet::AddEarPosition(int l, int r)
{
	string += "<message>POSITIONEAR</message>";
	string += "<leftposition>" + QString::number(l) + "</leftposition>";
	string += "<rightposition>" + QString::number(r) + "</rightposition>";
}

QByteArray ApiManager::ApiViolet::GetData()
{
	QString tmp("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
	tmp.append("<rsp>");
	tmp.append(GetInternalData());
	tmp.append("</rsp>");
	return tmp.toUtf8();
}
