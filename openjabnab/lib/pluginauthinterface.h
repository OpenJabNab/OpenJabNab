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

#include "pluginmanager.h"
inline PluginAuthInterface::PluginAuthInterface(QString name, QString visualName):PluginInterface(name, visualName, RequiredPlugin) {
	PluginManager::Instance().RegisterAuthPlugin(this);
}

inline PluginAuthInterface::~PluginAuthInterface()
{
	PluginManager::Instance().UnregisterAuthPlugin(this);
}

inline bool PluginAuthInterface::DoAuth(XmppHandler *, QByteArray const&, Bunny **, QByteArray &)
{
	// No auth available, disconnect
	return false;
}

Q_DECLARE_INTERFACE(PluginAuthInterface,"org.toms.openjabnab.PluginAuthInterface/1.0")

#endif
