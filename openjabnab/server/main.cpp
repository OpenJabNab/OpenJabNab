#include <iostream>
#include <signal.h>
#include "openjabnab.h"

OpenJabNab * o;

void sigCatcher(int)
{
	o->Close();
	QMetaObject::invokeMethod(o, "quit", Qt::QueuedConnection);
}

int main( int argc, char **argv )
{
	signal(SIGINT, sigCatcher);
	signal(SIGTERM, sigCatcher);

	o = new OpenJabNab(argc, argv);
	o->exec();
	delete o;

	return 0;
}
