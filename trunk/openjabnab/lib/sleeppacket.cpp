#include "sleeppacket.h"

SleepPacket::SleepPacket(State s)
{
	if (s == Sleep)
		sleep = true;
	else
		sleep = false;
}

SleepPacket * SleepPacket::Parse(QByteArray const& buffer)
{
	if(buffer.size() !=1)
		throw "Bad SleepPacket size : " + buffer.toHex();

	unsigned char value = (unsigned char)buffer.at(0);
	if (value > 1)
		throw "Bad SleepPacket value : " + buffer.toHex();

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

