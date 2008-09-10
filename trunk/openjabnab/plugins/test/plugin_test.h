#ifndef _PLUGINTEST_H_
#define _PLUGINTEST_H_

#include "plugininterface.h"
	
class PluginTest : public PluginInterface
{
	Q_OBJECT
	Q_INTERFACES(PluginInterface)

public:
	PluginTest();
	bool OnClick(Bunny *, PluginInterface::ClickType);
	bool HttpRequestHandle(HTTPRequest &);

private:
	int angle;
};

#endif
