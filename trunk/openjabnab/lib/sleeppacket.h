#ifndef _SLEEPPACKET_H_
#define _SLEEPPACKET_H_

#include <QByteArray>
#include "global.h"
#include "packet.h"

class OJN_EXPORT SleepPacket : public Packet
{
public:
	enum State { Wake_Up = 0, Sleep };
	
	SleepPacket() {};
	SleepPacket(State);
	static SleepPacket * Parse(QByteArray const&);
	~SleepPacket() {};


	QByteArray GetPrintableData() const;
	Packet_Types GetType() const { return Packet::Packet_Sleep; };
	State GetState() const { return (sleep==true?Sleep:Wake_Up); };

protected:
	QByteArray GetInternalData() const;
	bool sleep;

};

#endif
