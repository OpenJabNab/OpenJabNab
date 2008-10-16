#ifndef _MESSAGEPACKET_H_
#define _MESSAGEPACKET_H_

#include <QByteArray>
#include "global.h"
#include "packet.h"

class OJN_EXPORT MessagePacket : public Packet
{
public:
	MessagePacket(QByteArray const&);
	static MessagePacket * Parse(QByteArray const&);
	virtual ~MessagePacket() {};

	void SetMessage(QByteArray const& s) { message = s; };
	void AddMessage(QByteArray const& s) { message += s; };

	Packet_Types GetType() const { return Packet::Packet_Message; };
	QByteArray GetPrintableData() const { return message; };
	QByteArray const& GetMessage() const { return message; };
	
protected:
	MessagePacket();
	QByteArray GetInternalData() const;
	QByteArray message;
	
private:
	static const unsigned char inversion_table[];
};

#endif
