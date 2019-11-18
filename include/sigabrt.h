#ifndef HDDM_SIGABRT_H
#define HDDM_SIGABRT_H

#include <signal.h>
#include <sys/types.h>
#ifndef __USE_POSIX
#define __USE_POSIX // HOST_NAME_MAX
#endif
#include <limits.h>

#define SIGABRT_HANDLER_TEXT "Process is waiting to attach the debugger on host "

// Catch abort signals in the code scope.
class SigAbrtHandler
{
public :

	static void enable();
	static void disable();

private :

	static SigAbrtHandler& getInstance();

	SigAbrtHandler();
	SigAbrtHandler(SigAbrtHandler const&) = delete;
	void operator=(SigAbrtHandler const&) = delete;

public :

	~SigAbrtHandler();

private :

	bool enabled;

	const char text[sizeof(SIGABRT_HANDLER_TEXT)];
	char message[HOST_NAME_MAX + sizeof(SIGABRT_HANDLER_TEXT)];

	struct sigaction saOld;

	static void handler(int sig, siginfo_t *dont_care, void *dont_care_either);

	void enable_();
	void disable_();
};

#endif // HDDM_SIGABRT_H

