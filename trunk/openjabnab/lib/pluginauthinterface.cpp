#include "pluginauthinterface.h"
#include "xmpphandler.h"
#include "pluginmanager.h"


bool PluginAuthInterface::DoAuth(XmppHandler *, QByteArray const&, Bunny **, QByteArray &)
{
	// No auth available, disconnect
	return false;
}

PluginAuthInterface::PluginAuthInterface(QString name, QString visualName):PluginInterface(name, visualName, RequiredPlugin) {
	PluginManager::Instance().RegisterAuthPlugin(this);
}

PluginAuthInterface::~PluginAuthInterface()
{
	PluginManager::Instance().UnregisterAuthPlugin(this);
}
