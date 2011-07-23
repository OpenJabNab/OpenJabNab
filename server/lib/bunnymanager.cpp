#include "account.h"
#include "bunny.h"
#include "bunnymanager.h"
#include "httprequest.h"

BunnyManager::BunnyManager()
{
	bunniesDir = QCoreApplication::applicationDirPath();
	bunniesDir.cd("bunnies");
}

BunnyManager & BunnyManager::Instance()
{
  static BunnyManager b;
  return b;
}

void BunnyManager::LoadAllBunnies()
{
	LogInfo(QString("Finding bunnies in : %1").arg(bunniesDir.path()));
	QStringList filters;
	filters << "*.dat";
	bunniesDir.setNameFilters(filters);
	foreach (QFileInfo file, bunniesDir.entryInfoList(QDir::Files))
	{
		GetBunny(file.baseName().toAscii());
	}
}

QList<QByteArray> BunnyManager::GetConnectedBunniesList(void)
{
	QList<QByteArray> list;
	foreach(Bunny *b,Instance().listOfBunnies)
	{
		if(b->IsConnected()) {
			list.append(b->GetID());
		}
	}

	return list;
}

void BunnyManager::InitApiCalls()
{
	DECLARE_API_CALL("getListOfConnectedBunnies()", &BunnyManager::Api_GetListOfConnectedBunnies);
	DECLARE_API_CALL("getListOfBunnies()", &BunnyManager::Api_GetListOfBunnies);
	DECLARE_API_CALL("addBunny(serial)", &BunnyManager::Api_AddBunny);
	DECLARE_API_CALL("getListofAllBunnies()",&BunnyManager::Api_GetListOfAllBunnies);
	DECLARE_API_CALL("getListofAllConnectedBunnies()",&BunnyManager::Api_GetListOfAllConnectedBunnies);
}

int BunnyManager::GetConnectedBunnyCount()
{
	return GetConnectedBunnies().count();
}

int BunnyManager::GetBunnyCount()
{
	return listOfBunnies.count();
}

Bunny * BunnyManager::GetBunny(QByteArray const& bunnyHexID)
{
	QByteArray bunnyID = QByteArray::fromHex(bunnyHexID);

	if(listOfBunnies.contains(bunnyID))
		return listOfBunnies.value(bunnyID);

	Bunny * b = new Bunny(bunnyID);
	listOfBunnies.insert(bunnyID, b);
	return b;
}

Bunny * BunnyManager::GetBunny(PluginInterface * p, QByteArray const& bunnyHexID)
{
	Bunny * b = GetBunny(bunnyHexID);

	if(p->GetType() != PluginInterface::BunnyPlugin)
		return b;
	if(b->HasPlugin(p))
		return b;
	return NULL;
}

Bunny * BunnyManager::GetConnectedBunny(QByteArray const& bunnyHexID)
{
	QByteArray bunnyID = QByteArray::fromHex(bunnyHexID);

	if(listOfBunnies.contains(bunnyID))
	{
		Bunny * b = listOfBunnies.value(bunnyID);
		if(b->IsConnected())
			return b;
	}

	return NULL;
}

void BunnyManager::Close()
{
	foreach(Bunny * b, listOfBunnies)
		delete b;
	listOfBunnies.clear();
}

QVector<Bunny *> BunnyManager::GetConnectedBunnies()
{
	QVector<Bunny *> list;
	foreach(Bunny * b, listOfBunnies)
		if (b->IsConnected())
			list.append(b);
	return list;
}

void BunnyManager::PluginStateChanged(PluginInterface * p)
{
	foreach(Bunny * b, listOfBunnies)
		if (b->IsConnected())
			b->PluginStateChanged(p);
}

void BunnyManager::PluginLoaded(PluginInterface * p)
{
	foreach(Bunny * b, listOfBunnies)
		if (b->IsConnected())
			b->PluginLoaded(p);
}

void BunnyManager::PluginUnloaded(PluginInterface * p)
{
	foreach(Bunny * b, listOfBunnies)
		if (b->IsConnected())
			b->PluginUnloaded(p);
}


API_CALL(BunnyManager::Api_GetListOfConnectedBunnies)
{
	Q_UNUSED(hRequest);

	if(!account.HasAccess(Account::AcBunnies,Account::Read))
		return new ApiManager::ApiError("Access denied");

	QMap<QString, QVariant> list;
	foreach(Bunny * b, listOfBunnies)
		if(b->IsConnected() && account.GetBunniesList().contains(b->GetID()))
					list.insert(b->GetID(), b->GetBunnyName());

	return new ApiManager::ApiMappedList(list);
}

API_CALL(BunnyManager::Api_GetListOfBunnies) {
	Q_UNUSED(hRequest);

	if(!account.HasAccess(Account::AcBunnies,Account::Read))
		return new ApiManager::ApiError("Access denied");

	QMap<QString, QVariant> list;
	foreach(Bunny * b, listOfBunnies)
		if(account.GetBunniesList().contains(b->GetID()))
			list.insert(b->GetID(), b->GetBunnyName());

	return new ApiManager::ApiMappedList(list);
}

API_CALL(BunnyManager::Api_GetListOfAllBunnies) {
	Q_UNUSED(hRequest);

	if(!account.IsAdmin())
		return new ApiManager::ApiError("Access denied");

	QMap<QString, QVariant> list;
	foreach(Bunny * b, listOfBunnies)
		list.insert(b->GetID(), b->GetBunnyName());

	return new ApiManager::ApiMappedList(list);
}

API_CALL(BunnyManager::Api_GetListOfAllConnectedBunnies) {
	Q_UNUSED(hRequest);

	if(!account.IsAdmin())
		return new ApiManager::ApiError("Access denied");

	QMap<QString, QVariant> list;
	foreach(Bunny * b, listOfBunnies)
		if(b->IsConnected())
			list.insert(b->GetID(), b->GetBunnyName());

	return new ApiManager::ApiMappedList(list);
}

API_CALL(BunnyManager::Api_AddBunny) {
	if(!account.HasAccess(Account::AcBunnies,Account::Write))
		return new ApiManager::ApiError("Access denied");

	QByteArray bunnyID = hRequest.GetArg("serial").toAscii();
	if(listOfBunnies.contains(bunnyID))
		return new ApiManager::ApiError("Bunny already exists");

	GetBunny(bunnyID);
	return new ApiManager::ApiOk("Bunny successfully added");
}

QHash<QByteArray, Bunny *> BunnyManager::listOfBunnies;
