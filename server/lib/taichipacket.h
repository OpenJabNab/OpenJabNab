#ifndef _TAICHIPACKET_H_
#define _TAICHIPACKET_H_

#include <QByteArray>
#include "global.h"
#include "packet.h"

class OJN_EXPORT TaichiPacket : public Packet
{
public:
	TaichiPacket() {};
	static TaichiPacket * Parse(QByteArray const&);
	virtual ~TaichiPacket() {};

	QByteArray GetPrintableData() const;
	Packet_Types GetType() const;

protected:
	QByteArray GetInternalData() const;

};

inline Packet::Packet_Types TaichiPacket::GetType() const
{
	return Packet::Packet_Taichi;
}

#endif
