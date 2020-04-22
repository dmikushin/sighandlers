#include "sigabrt.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef _WIN32
#include <io.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

#ifndef STDERR_FILENO
#define STDERR_FILENO 2
#endif

#ifdef _WIN32
EXCEPTION_DISPOSITION SigAbrtHandler::handler(_EXCEPTION_RECORD* record, void* establisher_frame, _CONTEXT* context, void* dispatcher_context)
{
	SigAbrtHandler& sigAbrtHandler = SigAbrtHandler::getInstance();

	// Intervene, if environment variable is set.
	if (getenv("FREEZE_ON_ERROR"))
	{
		// Only basic write() is allowed within a signal handler (signal-safety).
		write(STDERR_FILENO, "Aborted\n", sizeof("Aborted\n"));

		// Freeze aborted process to allow e.g. attaching debugger.
		write(STDERR_FILENO, sigAbrtHandler.message, strlen(sigAbrtHandler.message));
		while (1) Sleep(1000); /* 1 sec */
	}
	else
	{
		// Display banner that one can re-run with FREEZE_ON_ERROR to debug.
		write(STDERR_FILENO, "You may want to set FREEZE_ON_ERROR environment variable to debug the case\n",
			sizeof("You may want to set FREEZE_ON_ERROR environment variable to debug the case\n")); 

#ifdef __MINGW32__
		__asm__(" \
		movl (%%esp),%%eax; \
		movl %%eax,%%fs:0; \
		addl $8,%%esp;" \
		: : : "%eax");
#elif _MSC_VER
		__asm
		{
			mov eax,[ESP]
			mov FS:[0], EAX
			add esp, 8
		}
#else
		#error "Unsupported compiler!"
#endif	

		return ExceptionContinueSearch;
	}
	
	return ExceptionContinueExecution;
}
#else
void SigAbrtHandler::handler(int sig, siginfo_t *dont_care, void *dont_care_either)
{
	SigAbrtHandler& sigAbrtHandler = SigAbrtHandler::getInstance();

	// Intervene, if environment variable is set.
	if (getenv("FREEZE_ON_ERROR"))
	{
		// Only basic write() is allowed within a signal handler (signal-safety).
		write(STDERR_FILENO, "Aborted\n", sizeof("Aborted\n"));

		// Freeze aborted process to allow e.g. attaching debugger.
		write(STDERR_FILENO, sigAbrtHandler.message, strlen(sigAbrtHandler.message));
		while (1) usleep(1000000); /* 1 sec */
	}
	else
	{
		// Display banner that one can re-run with FREEZE_ON_ERROR to debug.
		write(STDERR_FILENO, "You may want to set FREEZE_ON_ERROR environment variable to debug the case\n",
			sizeof("You may want to set FREEZE_ON_ERROR environment variable to debug the case\n")); 

		sigaction(SIGABRT, &sigAbrtHandler.saOld, NULL);
		raise(sig);
	}
}
#endif

void SigAbrtHandler::enable_()
{
	if (enabled) return;

#ifdef _WIN32
#ifdef __MINGW32__
	//DWORD hndlr = reinterpret_cast<DWORD>(&handler);
	
	__asm__(" \
	push %0; \
	push %%fs:0; \
	movl %%esp, %%fs:0;" : : "g" (handler));
#elif _MSC_VER
	DWORD hndlr = (DWORD)handler;
     
    __asm
    {
        push hndlr
        push FS:[0]
        mov FS:[0],ESP
    }
#else
	#error "Unsupported compiler!"
#endif
#else
	struct sigaction sa;

	memset(&sa, 0, sizeof(struct sigaction));
	sigemptyset(&sa.sa_mask);

	sa.sa_flags = SA_NODEFER;
	sa.sa_sigaction = handler;

	sigaction(SIGABRT, &sa, &saOld);
#endif

	enabled = true;
}

void SigAbrtHandler::disable_()
{
	if (!enabled) return;

#ifdef _WIN32
#ifdef __MINGW32__
	__asm__(" \
	movl (%%esp),%%eax; \
	movl %%eax,%%fs:0; \
	addl $8,%%esp;" \
	: : : "%eax");
#elif _MSC_VER
	__asm
    {
        mov eax,[ESP]
        mov FS:[0], EAX
        add esp, 8
    }
#else
	#error "Unsupported compiler!"
#endif
#else
	sigaction(SIGABRT, &saOld, NULL);
#endif

	enabled = false;
}

SigAbrtHandler::SigAbrtHandler() : enabled(false), text(SIGABRT_HANDLER_TEXT)
{
	// Record host name for e.g. distributed cluster applications.
	char hostname[HOST_NAME_MAX];
	gethostname(hostname, HOST_NAME_MAX);

	// Construct the static info message.
	memcpy(message, text, strlen(text));
	memcpy(message + strlen(text), hostname, strlen(hostname));
	memcpy(message + strlen(text) + strlen(hostname), "\n", sizeof("\n"));
}
	
SigAbrtHandler::~SigAbrtHandler()
{
	disable();
}

SigAbrtHandler& SigAbrtHandler::getInstance()
{
	static SigAbrtHandler instance;
	return instance;
}

void SigAbrtHandler::enable()
{
	SigAbrtHandler::getInstance().enable_();
}

void SigAbrtHandler::disable()
{
	SigAbrtHandler::getInstance().disable_();
}

