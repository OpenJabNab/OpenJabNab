#include "packet.h"
#include "ambientpacket.h"
#include "messagepacket.h"
#include "sleeppacket.h"
#include "log.h"

Packet * Packet::Parse(QByteArray const& buffer)
{
	// Check data
	const unsigned char * data = (const unsigned char *)buffer.constData();
	if (data[0] != 0x7F || data[buffer.size() - 1] != 0xFF)
		throw "Unable to parse packet : " + buffer.toHex();

	unsigned int len = data[2] << 16 | data[3] << 8 | data[4];
	if (buffer.size() != len + 6) // Header(1) + Type(1) + Len(3) + Trail(1)
		throw "Bad packet length : " + buffer.toHex();

	switch(data[1])
	{
		case Packet_Ambient:
			return AmbientPacket::Parse(buffer.mid(5, len));

		case Packet_Message:
			return MessagePacket::Parse(buffer.mid(5, len));
		
		case Packet_Sleep:
			return SleepPacket::Parse(buffer.mid(5, len));

		default:
			throw "Bad packet type : " + buffer.toHex();
	}
}

QByteArray Packet::GetData() const
{
	QByteArray tmp;
	QByteArray const& data = GetInternalData();
	tmp.append(0x7f);
	tmp.append(GetType());
	unsigned int len = data.size(); // Header(1) + Type(1) + Len(3) + Trail(1)
	tmp.append(len >> 16);
	tmp.append(len >> 8);
	tmp.append(len);
	tmp.append(data);
	tmp.append(0xFFu);
	return tmp;
}
