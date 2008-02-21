#ifndef _SLEEPPACKET_H_
#define _SLEEPPACKET_H_

#include <QByteArray>
#include "global.h"
#include "packet.h"

class OJN_EXPORT SleepPacket : public Packet
{
	enum State { Wake_Up = 0, Sleep };
	
public:
	SleepPacket() {};
	SleepPacket(State);
	static SleepPacket * Parse(QByteArray const&);
	~SleepPacket() {};
	QByteArray GetPrintableData() const;

	
protected:
	QByteArray GetInternalData() const;
	quint8 GetType() const { return Packet::Packet_Sleep; };

	bool sleep;
};

#endif
