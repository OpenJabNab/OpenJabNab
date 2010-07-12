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
	virtual ~SleepPacket() {};

	QByteArray GetPrintableData() const;
	Packet_Types GetType() const;

	State GetState() const;
	void SetState(State);
	
protected:
	QByteArray GetInternalData() const;
	bool sleep;

};

inline Packet::Packet_Types SleepPacket::GetType() const
{
	return Packet::Packet_Sleep;
}

inline SleepPacket::State SleepPacket::GetState() const
{
	if(sleep)
		return Sleep;
	else
		return Wake_Up;
}

inline void SleepPacket::SetState(State s)
{
	if (s == Sleep)
		sleep = true;
	else
		sleep = false;
}

#endif
