#include "ambientpacket.h"
#include "log.h"

AmbientPacket * AmbientPacket::Parse(QByteArray const& buffer)
{
	// Size should be a multiple of 2 (4bytes id & 2*N bytes for (ServiceID, ServiceValue))
	if ((buffer.size() - 4) % 2)
	{
		throw "Bad AmbientPacket size : " + buffer.toHex();
	}
	AmbientPacket * p = new AmbientPacket();
	for(int i = 4; i + 2 <= buffer.size(); i += 2)
	{
		p->services.insert(buffer.at(i), buffer.at(i+1));
	}
	return p;
}

AmbientPacket::AmbientPacket(enum Services s, unsigned char v)
{
	SetServiceValue(s,v);
}

void AmbientPacket::SetServiceValue(enum Services s, unsigned char v)
{
	services.insert(s,v);
}

void AmbientPacket::DisableService(enum Services s)
{
	services.remove(s);
	services.insert(Disable_Service, s);
}

QByteArray AmbientPacket::GetInternalData() const
{
	QByteArray tmp = QByteArray::fromHex("7FFFFFFE");
	QMapIterator<unsigned char, unsigned char> i(services);
	while (i.hasNext()) {
		i.next();
		tmp.append(i.key());
		tmp.append(i.value());
	}
	return tmp;
}

QByteArray AmbientPacket::GetPrintableData() const
{
	return GetHexData();
}