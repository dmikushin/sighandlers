#include <sigsegv.h>

#include <cstdio>

int main(int argc, char* argv[])
{
	SigSegvHandler::enable();

	int* ptr = NULL;
	*ptr = 1;

	return 0;
}

