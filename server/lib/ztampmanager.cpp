#include "account.h"
#include "ztamp.h"
#include "ztampmanager.h"
#include "httprequest.h"

ZtampManager::ZtampManager() {}

ZtampManager & ZtampManager::Instance()
{
  static ZtampManager z;
  return z;
}

void ZtampManager::InitApiCalls()
{
	DECLARE_API_CALL("getListOfZtamps()", &ZtampManager::Api_GetListOfZtamps);
}

Ztamp * ZtampManager::GetZtamp(QByteArray const& ztampHexID)
{
	QByteArray ztampID = QByteArray::fromHex(ztampHexID);
	
	if(listOfZtamps.contains(ztampID))
		return listOfZtamps.value(ztampID);

	Ztamp * z = new Ztamp(ztampID);
	listOfZtamps.insert(ztampID, z);
	return z;
}

Ztamp * ZtampManager::GetZtamp(PluginInterface * p, QByteArray const& ztampHexID)
{
	Ztamp * z = GetZtamp(ztampHexID);
	
	if(p->GetType() != PluginInterface::ZtampPlugin)
		return z;
	if(z->HasPlugin(p))
		return z;
	return NULL;
}

void ZtampManager::Close()
{
	foreach(Ztamp * z, listOfZtamps)
		delete z;
	listOfZtamps.clear();
}

QVector<Ztamp *> ZtampManager::GetZtamps()
{
	QVector<Ztamp *> list;
	foreach(Ztamp * z, listOfZtamps)
		list.append(z);
	return list;
}

void ZtampManager::PluginStateChanged(PluginInterface * p)
{
	foreach(Ztamp * z, listOfZtamps)
		z->PluginStateChanged(p);
}

void ZtampManager::PluginLoaded(PluginInterface * p)
{
	foreach(Ztamp * z, listOfZtamps)
		z->PluginLoaded(p);
}

void ZtampManager::PluginUnloaded(PluginInterface * p)
{
	foreach(Ztamp * z, listOfZtamps)
		z->PluginUnloaded(p);
}


API_CALL(ZtampManager::Api_GetListOfZtamps)
{
	Q_UNUSED(hRequest);

	if(!account.HasZtampsAccess(Account::Read))
		return new ApiManager::ApiError("Access denied");

	QMap<QString, QString> list;
	foreach(Ztamp * z, listOfZtamps)
		list.insert(z->GetID(), z->GetZtampName());

	return new ApiManager::ApiMappedList(list);
}

QHash<QByteArray, Ztamp *> ZtampManager::listOfZtamps;
