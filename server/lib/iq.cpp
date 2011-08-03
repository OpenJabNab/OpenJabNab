#include <QRegExp>
#include <QList>
#include "iq.h"

IQ::Iq_Types IQ::fromString(QByteArray const& type) const
{
	if(type == "get")
		return Iq_Get;
	if(type == "set")
		return Iq_Set;
	if(type == "result")
		return Iq_Result;
	return Iq_Unknown;
}

QByteArray IQ::toString(IQ::Iq_Types type) const
{
	switch(type)
	{
		case Iq_Get:
			return "get";
		case Iq_Set:
			return "set";
		case Iq_Result:
			return "result";
		default:
			return QByteArray();
	}
}

IQ::IQ(QByteArray const& data)
{
	// <iq from='...' to='...' type='...' id='...'>...</iq>
	QRegExp rx("<iq ([^>]*)>(.*)</iq>");
	if(rx.indexIn(data) != -1)
	{
		isValid = true;
		content = rx.cap(2).toAscii();
		// Now split args
		QList<QByteArray> args = rx.cap(1).toAscii().split(' ');
		foreach(QByteArray arg, args)
		{
			int i = arg.indexOf('=');
			if (i != -1)
			{
				QByteArray key = arg.left(i);
				QByteArray value = arg.mid(i+2);
				value.chop(1); // Last '
				if(key == "type")
					type = fromString(value);
				else if (key == "id")
					id = value;
				else if (key == "from")
					from = value;
				else if (key == "to")
					to = value;
				else
				{
					isValid = false;
					break;
				}
			}
		}
	}
	else
		isValid = false;
}

// %1 = id, %2 = from, %3 = to, %4 = result
QByteArray IQ::Reply(Iq_Types newType, QByteArray const& format, QByteArray const& content)
{
	// IQ Parameters, from & to are swapped (reply)
	QString sid = "id='" + id + "'";
	QString stype = "type='" + toString(newType) + "'";
	QString sfrom = "from='" + to + "'";
	QString sto = "to='" + from + "'";

	QByteArray reply = QString("<iq ").append(QString(format).replace("%1", sid).replace("%2", sfrom).replace("%3", sto).replace("%4", stype)).toAscii();

	if(content.isNull())
	{
		reply.append("/>");
	}
	else
	{
		reply.append(">");
		reply.append(content);
		reply.append("</iq>");
	}

	return reply;
}
