#include "messagepacket.h"

MessagePacket::MessagePacket() {}

MessagePacket::MessagePacket(QByteArray const& m):message(m) {}

MessagePacket * MessagePacket::Parse(QByteArray const& buffer)
{
	MessagePacket * p = new MessagePacket();
	// Deobfuscating algorithm by Sache
	unsigned char currentChar = 35;
	for(int i = 1; i < buffer.size(); i++)
	{
		unsigned char code = (unsigned char)buffer.at(i);
		currentChar = ((code-47)*(1+2*currentChar))%256;
		p->message.append(currentChar);
	}
	return p;
}

QByteArray MessagePacket::GetInternalData() const
{
	QByteArray tmp;
	// Obfuscating algorithm by Sache
	unsigned char previousChar = 35;
	tmp.append((char)0x00);
	for(int i = 0; i < message.size(); i++)
	{
		unsigned char currentChar = message.at(i);
		tmp.append(inversion_table[previousChar % 128] * currentChar + 47);
		previousChar = currentChar;
	}
	return tmp;
}

const unsigned char MessagePacket::inversion_table[] = { 1, 171, 205, 183, 57, 163, 197, 239, 241, 27, 61, 167, 41, 19, 53, 223, 225, 139, 173, 151, 25, 131, 165, 207, 209, 251, 29, 135, 9, 243, 21, 191, 193, 107, 141, 119, 249, 99, 133, 175, 177, 219, 253, 103, 233, 211, 245, 159, 161, 75, 109, 87, 217, 67, 101, 143, 145, 187, 221, 71, 201, 179, 213, 127, 129, 43, 77, 55, 185, 35, 69, 111, 113, 155, 189, 39, 169, 147, 181, 95, 97,11, 45, 23, 153, 3, 37, 79, 81, 123, 157, 7, 137, 115, 149, 63, 65, 235, 13, 247, 121, 227, 5, 47, 49, 91, 125, 231, 105, 83, 117, 31, 33, 203, 237, 215, 89, 195, 229, 15, 17, 59, 93, 199, 73, 51, 85, 255 };
