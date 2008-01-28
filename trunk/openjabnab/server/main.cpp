#include <iostream>
#include "openjabnab.h"
#include <signal.h>

OpenJabNab * o;

void sigCatcher(int sig)
{
	delete o;
	exit(0);
}

int main( int argc, char **argv )
{
	signal(SIGINT, sigCatcher);
	signal(SIGTERM, sigCatcher);

	o = new OpenJabNab(argc, argv);
	o->exec();

	return 0;
}
