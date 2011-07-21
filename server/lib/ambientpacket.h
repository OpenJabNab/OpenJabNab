#ifndef _AMBIENTPACKET_H_
#define _AMBIENTPACKET_H_

#include <QByteArray>
#include <QMap>
#include "global.h"
#include "packet.h"

class OJN_EXPORT AmbientPacket : public Packet
{
public:
	enum Services { Disable_Service = 0, Service_Weather, Service_StockMarket, Service_Periph, MoveLeftEar, MoveRightEar, Service_EMail, Service_AirQuality, Service_Nose, Service_TaiChi = 0x0e};
	enum Weather_Values { Weather_Sun = 0, Weather_Cloudy, Weather_Smog, Weather_Rain, Weather_Snow, Weather_Storm};
	enum StockMarket_Values { StockMarket_HighDown = 0, StockMarket_MediumDown, StockMarket_LittleDown, StockMarket_Stable, StockMarket_LittleUp, StockMarket_MediumUp, StockMarket_HighUp};
	enum EMail_Values { EMail_No = 0, EMail_1, EMail_2, EMail_3AndMore };
	enum Nose_Values { Nose_No = 0, Nose_Blink, Nose_DoubleBlink };

	AmbientPacket() {};
	AmbientPacket(enum Services, unsigned char value);
	static AmbientPacket * Parse(QByteArray const&);
	virtual ~AmbientPacket() {};

	void SetServiceValue(enum Services, unsigned char);
	void SetEarsPosition(unsigned char, unsigned char);
	void DisableService(enum Services);

	Packet_Types GetType() const { return Packet::Packet_Ambient; };
	QByteArray GetPrintableData() const;
	QMap<unsigned char, unsigned char> const& GetServices() { return services; };
	
protected:
	QByteArray GetInternalData() const;
	QMap<unsigned char, unsigned char> services;
};

#endif
