#ifndef _AMBIENTPACKET_H_
#define _AMBIENTPACKET_H_

#include <QByteArray>
#include <QMap>

#include "packet.h"
#include "global.h"

class OJN_EXPORT AmbientPacket : public Packet
{
	enum Services { Disable_Service = 0, Service_Weather, Service_Bourse, Service_Periph, MoveLeftEar, MoveRightEar, Service_Mail, Service_AirQuality, Service_Noze};

public:
	AmbientPacket(QByteArray const&);
	~AmbientPacket() {};
	QByteArray GetPrintableData();

	void SetServiceValue(enum Services,unsigned char);
	
protected:
	QByteArray GetInternalData();
	quint8 GetType() { return 0x04; };
	QMap<unsigned char, unsigned char> services;
};

#endif
