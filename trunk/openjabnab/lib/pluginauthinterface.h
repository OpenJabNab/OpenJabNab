#ifndef _PLUGINAUTHINTERFACE_H_
#define _PLUGINAUTHINTERFACE_H_

#include <QByteArray>
#include "plugininterface.h"

class Bunny;
class XmppHandler;
class PluginAuthInterface : public PluginInterface
{
public:
	PluginAuthInterface(QString name, QString visualName = QString());
	virtual ~PluginAuthInterface();

	virtual bool DoAuth(XmppHandler *, QByteArray const&, Bunny **, QByteArray &);

	// Static
	static PluginAuthInterface * DummyPlugin();
};

inline PluginAuthInterface * PluginAuthInterface::DummyPlugin()
{
	static PluginAuthInterface p("dummy", "dummy");
	return &p;
}

Q_DECLARE_INTERFACE(PluginAuthInterface,"org.toms.openjabnab.PluginAuthInterface/1.0")

#endif
