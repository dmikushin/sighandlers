#ifndef HDDM_SIGSEGV_H
#define HDDM_SIGSEGV_H

#include <signal.h>
#include <sys/types.h>
#ifndef __USE_POSIX
#define __USE_POSIX // HOST_NAME_MAX
#endif
#include <limits.h>

#define SIGSEGV_HANDLER_TEXT "Process is waiting to attach the debugger on host "

// Catch segmentation fault signals in the code scope.
class SigSegvHandler
{
public :

	static void enable();
	static void disable();

private :

	static SigSegvHandler& getInstance();

	SigSegvHandler();
	SigSegvHandler(SigSegvHandler const&) = delete;
	void operator=(SigSegvHandler const&) = delete;

public :

	~SigSegvHandler();

private :

	bool enabled;

	const char text[sizeof(SIGSEGV_HANDLER_TEXT)];
	char message[HOST_NAME_MAX + sizeof(SIGSEGV_HANDLER_TEXT)];

	struct sigaction saOld;

	static void handler(int sig, siginfo_t *dont_care, void *dont_care_either);

	void enable_();
	void disable_();
};

#endif // HDDM_SIGSEGV_H

