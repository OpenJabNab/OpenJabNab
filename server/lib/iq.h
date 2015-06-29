#ifndef _IQ_H_
#define _IQ_H_

#include <QByteArray>
#include "global.h"

class OJN_EXPORT IQ
{
public:
	enum Iq_Types { Iq_Get, Iq_Set, Iq_Result, Iq_Unknown };
	IQ(QByteArray const&);
	~IQ() {};

	bool IsValid() const;
	IQ::Iq_Types Type() const;
	QByteArray const& Content() const;
	QByteArray const& From() const;
	// %1 = id, %2 = from, %3 = to, %4 = result
	QByteArray Reply(Iq_Types type, QByteArray const&, QByteArray const& content);
	
protected:

	Iq_Types fromString(QByteArray const&) const;
	QByteArray toString(IQ::Iq_Types type) const;

	bool isValid;
	QByteArray from;
	QByteArray to;
	Iq_Types type;
	QByteArray id;
	QByteArray content;
};

inline bool IQ::IsValid() const
{
	return isValid;
}

inline IQ::Iq_Types IQ::Type() const
{
	return type;
}

inline QByteArray const& IQ::Content() const
{
	return content;
}

inline QByteArray const& IQ::From() const
{
	return from;
}

#endif
