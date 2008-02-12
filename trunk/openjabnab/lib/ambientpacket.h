#ifndef _AMBIENTPACKET_H_
#define _AMBIENTPACKET_H_

#include <QByteArray>
#include <QMap>

#include "packet.h"
#include "global.h"

class OJN_EXPORT AmbientPacket : public Packet
{
public:
	enum Services { Disable_Service = 0, Service_Weather, Service_StockMarket, Service_Periph, MoveLeftEar, MoveRightEar, Service_EMail, Service_AirQuality, Service_Noze, Service_TaiChi = 0x0e};
	enum Weather_Values { Weather_Sun = 0, Weather_Cloudy, Weather_Smog, Weather_Rain, Weather_Snow, Weather_Storm};
	enum StockMarket_Values { StockMarket_HighDown = 0, StockMarket_MediumDown, StockMarket_LittleDown, StockMarket_Stable, StockMarket_LittleUp, StockMarket_MediumUp, StockMarket_HighUp};
	enum EMail_Values { EMail_No = 0, EMail_1, EMail_2, EMail_3AndMore };
	enum Noze_Values { Noze_No = 0, Noze_Blink, Noze_DoubleBlink };

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
