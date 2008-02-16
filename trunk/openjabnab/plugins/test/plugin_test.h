#ifndef _PLUGINTEST_H_
#define _PLUGINTEST_H_

#include <QObject>
#include "plugininterface.h"
	
class PluginTest : public QObject, public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	PluginTest();
	bool OnClick(Bunny *, PluginInterface::ClickType);

private:
	int angle;
};

#endif
