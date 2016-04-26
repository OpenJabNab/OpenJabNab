#include <QString>
#include "taichipacket.h"

TaichiPacket * TaichiPacket::Parse(QByteArray const& buffer)
{
	if(buffer.size()!= 0)
		throw QString("Bad TaichiPacket size : %1").arg(QString(buffer.toHex()));

	return new TaichiPacket;
}

QByteArray TaichiPacket::GetInternalData() const
{
	return QByteArray();
}

QByteArray TaichiPacket::GetPrintableData() const
{
	return "none";
}
