#ifndef _CHOREGRAPHY_H_
#define _CHOREGRAPHY_H_

#include <QByteArray>
#include <QMap>
#include "global.h"

class OJN_EXPORT Choregraphy
{
public:
	enum Direction { Dir_Forward = 0, Dir_Backward };
	enum Ear { Ear_Left = 0, Ear_Right };
	enum Led { Led_Bottom = 0, Led_Left, Led_Middle, Led_Right,	Led_Top };

	// Set Tempo in ms
	void SetTempo(unsigned int);
	// Set Tempo in Hz
	void SetFreq(unsigned int);
	
	void AddLedAction(unsigned int, enum Led, quint8, quint8, quint8);
	void AddMotorAction(unsigned int, enum Ear, unsigned int, enum Direction);
	QByteArray GetData();
	bool Parse(QString);

private:
	class Action
	{
		public:
			virtual QByteArray GetData() = 0;
			virtual ~Action() {} ;
	};
	
	class MotorAction : public Action
	{
		public:
			MotorAction(enum Ear, unsigned int, enum Direction);
			QByteArray GetData();

		private:
			enum Ear ear;
			unsigned int angle;
			enum Direction dir;
	};

	class LedAction : public Action
	{
		public:
			LedAction(enum Led, quint8, quint8, quint8);
			QByteArray GetData();

		private:
			enum Led led;
			quint8 red, green, blue;
	};

	unsigned int tempo; // Nb of ms between two actions
	QMap<unsigned int, Action *> listOfActions;
};

#endif
