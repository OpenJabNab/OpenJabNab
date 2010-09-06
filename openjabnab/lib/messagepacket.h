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

	void SetMessage(QByteArray const& s);
	void AddMessage(QByteArray const& s);

	Packet_Types GetType() const;
	QByteArray GetPrintableData() const;
	QByteArray const& GetMessage() const;
	
protected:
	MessagePacket();
	QByteArray GetInternalData() const;
	QByteArray message;
	
private:
	static const unsigned char inversion_table[];
};


inline void MessagePacket::SetMessage(QByteArray const& s)
{
	message = s;
}

inline void MessagePacket::AddMessage(QByteArray const& s)
{
	message += s;
}

inline Packet::Packet_Types MessagePacket::GetType() const
{
	return Packet::Packet_Message;
}

inline QByteArray MessagePacket::GetPrintableData() const
{
	return message;
}

inline QByteArray const& MessagePacket::GetMessage() const
{
	return message;
}

#endif
