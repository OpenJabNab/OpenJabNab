#ifndef _MESSAGEPACKET_H_
#define _MESSAGEPACKET_H_

#include <QByteArray>
#include "global.h"
#include "packet.h"

class OJN_EXPORT MessagePacket : public Packet
{

public:
	static MessagePacket * Parse(QByteArray const&);
	~MessagePacket() {};
	QByteArray GetPrintableData() const;

	
protected:
	QByteArray GetInternalData() const;
	quint8 GetType() const { return Packet::Packet_Message; };

	QByteArray message;
	
private:
	static const unsigned char inversion_table[];
};

#endif
