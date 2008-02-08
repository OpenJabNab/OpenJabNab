#ifndef _AMBIENTPACKET_H_
#define _AMBIENTPACKET_H_

#include <QByteArray>
#include <QMap>

#include "packet.h"
#include "global.h"

class OJN_EXPORT AmbientPacket : public Packet
{
public:
	enum Services { Disable_Service = 0, Service_Weather, Service_Bourse, Service_Periph, MoveLeftEar, MoveRightEar, Service_Mail, Service_AirQuality, Service_Noze};
	enum Weather_Values { Weather_Sun = 0, Weather_Cloudy, Weather_B, Weather_Rain, Weather_Snow, Weather_O};

	AmbientPacket() {};
	AmbientPacket(enum Services, unsigned char value);
	~AmbientPacket() {};

	QByteArray GetPrintableData() const;
	void SetServiceValue(enum Services, unsigned char);
	void DisableService(enum Services);
	static AmbientPacket * Parse(QByteArray const&);

	
protected:
	QByteArray GetInternalData() const;
	quint8 GetType() const { return Packet::Packet_Ambient; };
	QMap<unsigned char, unsigned char> services;
};

#endif
