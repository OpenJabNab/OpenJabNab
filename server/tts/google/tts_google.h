#ifndef _TTSGOOGLE_H_
#define _TTSGOOGLE_H_

#include <QHttp>
#include <QMultiMap>
#include <QTextStream>
#include <QThread>
#include "ttsinterface.h"
	
class TTSGoogle : public TTSInterface
{
	Q_OBJECT
	Q_INTERFACES(TTSInterface)
	
public:
	TTSGoogle();
	virtual ~TTSGoogle();
	QByteArray CreateNewSound(QString, QString, bool);

private:
};

#endif
