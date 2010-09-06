#include "ambientpacket.h"
#include "messagepacket.h"
#include "packet.h"
#include "sleeppacket.h"
#include "log.h"

QList<Packet*> Packet::Parse(QByteArray const& originalBuffer)
{
	QList<Packet*> list;
	QByteArray buffer = originalBuffer;

	// Check data
	const unsigned char * data = (const unsigned char *)buffer.constData();
	if (data[0] != 0x7F || data[buffer.size() - 1] != 0xFF)
		throw QString("Unable to parse packet : %1").arg(QString(buffer.toHex()));

	buffer.remove(0,1); // Removes 1st byte (7F)
	while(buffer.size() != 1)
	{
		Packet * p;
		data = (const unsigned char *)buffer.constData();
		int len = data[1] << 16 | data[2] << 8 | data[3];
		if (buffer.size() < len + 5) // Type(1) + Len(3) + Trail(1)
			throw QString("Bad packet length : %1 / %2").arg(QString(buffer.toHex()),QString(originalBuffer.toHex()));

		switch(data[0])
		{
			case Packet_Ambient:
				p = AmbientPacket::Parse(buffer.mid(4, len));
				break;

			case Packet_Message:
				p = MessagePacket::Parse(buffer.mid(4, len));
				break;
			
			case Packet_Sleep:
				p = SleepPacket::Parse(buffer.mid(4, len));
				break;

			default:
				throw QString("Bad packet type : %1 / %2").arg(QString(buffer.toHex()),QString(originalBuffer.toHex()));
		}
		list.append(p);
		buffer.remove(0, len + 4); // Type(1) + Len(3)
	}
	return list;
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

QByteArray Packet::GetData(QList<Packet*> const& list)
{
	QByteArray tmp;
	tmp.append(0x7f);
	foreach(Packet * p, list)
	{
		QByteArray const& data = p->GetInternalData();
		tmp.append(p->GetType());
		unsigned int len = data.size(); // Header(1) + Type(1) + Len(3) + Trail(1)
		tmp.append(len >> 16);
		tmp.append(len >> 8);
		tmp.append(len);
		tmp.append(data);
	}
	tmp.append(0xFFu);
	return tmp;
}
