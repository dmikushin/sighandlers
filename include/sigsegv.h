#ifndef HDDM_SIGSEGV_H
#define HDDM_SIGSEGV_H

#include <signal.h>
#include <sys/types.h>
#ifndef __USE_POSIX
#define __USE_POSIX // HOST_NAME_MAX
#endif
#include <limits.h>

#ifdef _WIN32
#include "windows.h"
#include "excpt.h"

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 64
#endif
#endif

#ifndef HOST_NAME_MAX
# if defined(_POSIX_HOST_NAME_MAX)
#  define HOST_NAME_MAX _POSIX_HOST_NAME_MAX
# elif defined(MAXHOSTNAMELEN)
#  define HOST_NAME_MAX MAXHOSTNAMELEN
# endif
#endif /* HOST_NAME_MAX */

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

#ifdef _WIN32
	static EXCEPTION_DISPOSITION handler(_EXCEPTION_RECORD* record, void* establisher_frame, _CONTEXT* context, void* dispatcher_context);
#else
	struct sigaction saOld;

	static void handler(int sig, siginfo_t *dont_care, void *dont_care_either);
#endif

	void enable_();
	void disable_();
};

#endif // HDDM_SIGSEGV_H

