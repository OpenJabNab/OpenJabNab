#include "messagepacket.h"
#include "stdio.h"

MessagePacket::MessagePacket(QByteArray const& buffer)
{
// Deobfuscating algorithm by Sache
	unsigned char currentChar = 35;
	for(int i = 1; i < buffer.size(); i++)
	{
		unsigned char code = (unsigned char)buffer.at(i);
		currentChar = ((code-47)*(1+2*currentChar))%256;
		message.append(currentChar);
	}
}

QByteArray MessagePacket::GetInternalData()
{
	return QByteArray();
}

QByteArray MessagePacket::GetPrintableData()
{
	return message;
}