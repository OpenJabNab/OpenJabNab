#include <iostream>
#include <signal.h>
#include "openjabnab.h"

OpenJabNab * o;

void sigCatcher(int)
{
	o->quit();
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
