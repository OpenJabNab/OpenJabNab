#include "ambientpacket.h"
#include "stdio.h"

AmbientPacket::AmbientPacket(QByteArray const& buffer)
{
	for(int i = 4; i + 2 <= buffer.size(); i += 2)
	{
		services.insert(buffer.at(i), buffer.at(i+1));
	}
}

void AmbientPacket::SetServiceValue(enum Services k,unsigned char v)
{
	services.insert(k,v);
}

QByteArray AmbientPacket::GetInternalData()
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

QByteArray AmbientPacket::GetPrintableData()
{
	return GetHexData();
}