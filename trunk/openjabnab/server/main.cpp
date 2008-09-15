#include <iostream>
#include <signal.h>
#include "openjabnab.h"

OpenJabNab * o;

void sigCatcher(int)
{
	OpenJabNab::exit();
}

int main( int argc, char **argv )
{
	signal(SIGINT, sigCatcher);
	signal(SIGTERM, sigCatcher);

	o = new OpenJabNab(argc, argv);
	o->exec();

	return 0;
}
