#include "account.h"
#include "bunny.h"
#include "bunnymanager.h"
#include "httprequest.h"

BunnyManager::BunnyManager() {}

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


ApiManager::ApiAnswer * BunnyManager::ProcessApiCall(Account const& account, QByteArray const& request, HTTPRequest const& hRequest)
{
	if (request == "getListOfConnectedBunnies")
	{
		if(!account.HasBunniesAccess(Account::Read))
			return new ApiManager::ApiError(QByteArray("Access denied"));
		QMap<QByteArray, QByteArray> list;
		foreach(Bunny * b, listOfBunnies)
			if (b->IsConnected())
				list.insert(b->GetID(), b->GetBunnyName().toAscii());
		return new ApiManager::ApiMappedList(list);
	}
	else
		return new ApiManager::ApiError("Unknown Bunnies Api Call : " + request + "<br />Request was : " + hRequest.toString());
}

QMap<QByteArray, Bunny *> BunnyManager::listOfBunnies;
