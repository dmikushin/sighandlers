#include <sigabrt.h>

#include <cstdlib>

int main(int argc, char* argv[])
{
	SigAbrtHandler::enable();

	abort();

	return 0;
}

