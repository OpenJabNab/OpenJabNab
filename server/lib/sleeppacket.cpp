#include <QString>
#include "sleeppacket.h"

SleepPacket::SleepPacket(State s)
{
	SetState(s);
}

SleepPacket * SleepPacket::Parse(QByteArray const& buffer)
{
	if(buffer.size() !=1)
		throw QString("Bad SleepPacket size : %1").arg(QString(buffer.toHex()));

	unsigned char value = (unsigned char)buffer.at(0);
	if (value > 1)
		throw QString("Bad SleepPacket value : %1").arg(QString(buffer.toHex()));

	SleepPacket * s = new SleepPacket;
	s->sleep = value;
	return s;
}

QByteArray SleepPacket::GetInternalData() const
{
	return QByteArray(1,sleep);
}

QByteArray SleepPacket::GetPrintableData() const
{
	if(sleep)
		return "Go to sleep";
	else
		return "Wake up";
}
