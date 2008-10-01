#ifndef _PACKET_H_
#define _PACKET_H_

#include <QByteArray>
#include "global.h"

class OJN_EXPORT Packet
{
public:
	enum Packet_Types { Packet_Ambient = 0x04, Packet_Message = 0x0A, Packet_Sleep = 0x0B };
	
	virtual ~Packet() {};
	static Packet * Parse(QByteArray const&);
	QByteArray GetData() const;
	QByteArray GetHexData() const { return GetData().toHex(); }
	virtual QByteArray GetPrintableData() const = 0;
	virtual Packet_Types GetType() const = 0;
	
protected:
	virtual QByteArray GetInternalData() const = 0;
};

#endif
