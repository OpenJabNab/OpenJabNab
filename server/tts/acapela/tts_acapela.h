#ifndef _TTSACAPELA_H_
#define _TTSACAPELA_H_

#include <QHttp>
#include <QMultiMap>
#include <QTextStream>
#include <QThread>
#include "ttsinterface.h"
	
class TTSAcapela : public TTSInterface
{
	Q_OBJECT
	Q_INTERFACES(TTSInterface)
	
public:
	TTSAcapela();
	virtual ~TTSAcapela();
	QByteArray CreateNewSound(QString, QString, bool);

private:
};

#endif
