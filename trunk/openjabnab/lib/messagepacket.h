#ifndef _MESSAGEPACKET_H_
#define _MESSAGEPACKET_H_

#include <QByteArray>

#include "packet.h"
#include "global.h"

class OJN_EXPORT MessagePacket : public Packet
{

public:
	MessagePacket(QByteArray const&);
	~MessagePacket() {};
	QByteArray GetPrintableData();

	
protected:
	QByteArray GetInternalData();
	quint8 GetType() { return 0x0A; };

	QByteArray message;
};

#endif
