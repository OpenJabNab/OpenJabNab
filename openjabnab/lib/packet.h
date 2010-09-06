#ifndef _PACKET_H_
#define _PACKET_H_

#include <QByteArray>
#include <QList>
#include "global.h"

class OJN_EXPORT Packet
{
public:
	enum Packet_Types { Packet_Ambient = 0x04, Packet_Message = 0x0A, Packet_Sleep = 0x0B };
	
	virtual ~Packet() {};
	static QList<Packet *> Parse(QByteArray const&);
	QByteArray GetData() const;
	QByteArray GetHexData() const;
	static QByteArray GetData(QList<Packet*> const&);
	static QByteArray GetHexData(QList<Packet*> const&);
	virtual QByteArray GetPrintableData() const = 0;
	virtual Packet_Types GetType() const = 0;
	
protected:
	virtual QByteArray GetInternalData() const = 0;
};

inline QByteArray Packet::GetHexData() const
{
	return GetData().toHex();
}

#endif
