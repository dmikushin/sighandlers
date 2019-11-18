#include "sigsegv.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <pthread.h>
#include <unistd.h>

void SigSegvHandler::handler(int sig, siginfo_t *dont_care, void *dont_care_either)
{
	SigSegvHandler& SigSegvHandler = SigSegvHandler::getInstance();

	// Intervene, if environment variable is set.
	if (getenv("FREEZE_ON_ERROR"))
	{
		// Only basic write() is allowed within a signal handler (signal-safety).
		write(STDERR_FILENO, "Segmentation fault\n", sizeof("Segmentation fault\n"));

		// Freeze aborted process to allow e.g. attaching debugger.
		write(STDERR_FILENO, SigSegvHandler.message, strlen(SigSegvHandler.message));
		while (1) usleep(1000000); /* 1 sec */
	}
	else
	{
		// Display banner that one can re-run with FREEZE_ON_ERROR to debug.
		write(STDERR_FILENO, "You may want to set FREEZE_ON_ERROR environment variable to debug the case\n",
			sizeof("You may want to set FREEZE_ON_ERROR environment variable to debug the case\n")); 

		sigaction(SIGSEGV, &SigSegvHandler.saOld, NULL);
		raise(sig);
	}
}

void SigSegvHandler::enable_()
{
	if (enabled) return;

	struct sigaction sa;

	memset(&sa, 0, sizeof(struct sigaction));
	sigemptyset(&sa.sa_mask);

	sa.sa_flags = SA_NODEFER;
	sa.sa_sigaction = handler;

	sigaction(SIGSEGV, &sa, &saOld);

	enabled = true;
}

void SigSegvHandler::disable_()
{
	if (!enabled) return;

	sigaction(SIGSEGV, &saOld, NULL);
	enabled = false;
}

SigSegvHandler::SigSegvHandler() : enabled(false), text(SIGSEGV_HANDLER_TEXT)
{
	// Record host name for e.g. distributed cluster applications.
	char hostname[HOST_NAME_MAX];
	gethostname(hostname, HOST_NAME_MAX);

	// Construct the static info message.
	memcpy(message, text, strlen(text));
	memcpy(message + strlen(text), hostname, strlen(hostname));
	memcpy(message + strlen(text) + strlen(hostname), "\n", sizeof("\n"));
}
	
SigSegvHandler::~SigSegvHandler()
{
	disable();
}

SigSegvHandler& SigSegvHandler::getInstance()
{
	static SigSegvHandler instance;
	return instance;
}

void SigSegvHandler::enable()
{
	SigSegvHandler::getInstance().enable_();
}

void SigSegvHandler::disable()
{
	SigSegvHandler::getInstance().disable_();
}

