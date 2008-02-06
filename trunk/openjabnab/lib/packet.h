#ifndef _PACKET_H_
#define _PACKET_H_

#include <QByteArray>

#include "global.h"

class OJN_EXPORT Packet
{
public:
	virtual ~Packet() {};
	static Packet * Parse(QByteArray const&);
	QByteArray GetData();
	QByteArray GetHexData() { return GetData().toHex(); }
	virtual QByteArray GetPrintableData() = 0;
	
protected:
	virtual QByteArray GetInternalData() = 0;
	virtual quint8 GetType() = 0;
};

#endif
