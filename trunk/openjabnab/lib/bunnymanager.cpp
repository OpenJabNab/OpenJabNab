#include "bunny.h"
#include "bunnymanager.h"

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

QMap<QByteArray, Bunny *> BunnyManager::listOfBunnies;
