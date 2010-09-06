#include "choregraphy.h"
#include "log.h"

void Choregraphy::SetTempo(unsigned int t)
{
	tempo = t;
}

void Choregraphy::SetFreq(unsigned int t)
{
	if (t)
		tempo = 1000/t;
}

QByteArray Choregraphy::GetData()
{
	unsigned char t;
	if (tempo > 2550)
	{
		LogWarning("Choregraphy::GetData, tempo too big !");
		t = 0xFF;
	}
	else if (tempo < 10)
	{
		LogWarning("Choregraphy::GetData, tempo too small !");
		t = 0x01;
	}
	else
		t = (tempo / 10);
	
	QByteArray tmp(4,0); // Allocate size, will be set later
	// Set Tempo
	tmp.append((char)0x00);
	tmp.append(0x01);
	tmp.append((unsigned char)t);
	// Add each "action"
	QMap<unsigned int, Action *>::const_iterator i;
	unsigned int lastIndex = 0;
	for (i = listOfActions.constBegin(); i != listOfActions.constEnd(); ++i)
	{
		unsigned int currentIndex = i.key() - lastIndex;
		if (currentIndex > 255)
		{
			LogWarning("Choregraphy::GetData, wait too long !");
			currentIndex = 255;
		}
		tmp.append((unsigned char)currentIndex);
		tmp.append(i.value()->GetData());
		lastIndex = i.key();
	}
	// Set Len
	unsigned int len = tmp.size() - 4;
	tmp[0] = (len >> 24);
	tmp[1] = (len >> 16);
	tmp[2] = (len >> 8);
	tmp[3] = (len);
	// Trailer ?
	tmp.append(QByteArray(4,0));
	return tmp;
}

void Choregraphy::AddLedAction(unsigned int time, enum Led l, quint8 r, quint8 g, quint8 b)
{
	listOfActions.insertMulti(time, new LedAction(l, r, g, b));
}

void Choregraphy::AddMotorAction(unsigned int time, enum Ear e, unsigned int a, enum Direction d)
{
	listOfActions.insertMulti(time, new MotorAction(e,a,d));
}

Choregraphy::MotorAction::MotorAction(enum Ear e, unsigned int a, enum Direction d):ear(e),angle(a),dir(d)
{}

Choregraphy::LedAction::LedAction(enum Led l, quint8 r, quint8 g, quint8 b):led(l),red(r),green(g),blue(b)
{}

QByteArray Choregraphy::MotorAction::GetData()
{
	QByteArray(tmp);
	tmp.append(0x08); // MotorAction
	tmp.append(ear);
	tmp.append(angle/18);
	tmp.append(dir);
	return tmp;
}

QByteArray Choregraphy::LedAction::GetData()
{
	QByteArray(tmp);
	tmp.append(0x07); // LedAction
	tmp.append(led);
	tmp.append(red);
	tmp.append(green);
	tmp.append(blue);
	tmp.append((char)0x00);
	tmp.append((char)0x00);
	return tmp;
}

