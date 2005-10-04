// Copyright (c) 2003-2006 by Mike Sharov <msharov@users.sourceforge.net>
//
// startup.cc
//
// Static signal and exit handlers.
//

#define _GNU_SOURCE 1 // for strsignal (optional)
#include "startup.h"
#include "app.h"
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

namespace fbgl {

//----------------------------------------------------------------------

static void OnSignal (int sig, siginfo* si, void* oldHandler);
static void OnUnexpected (void) __attribute__((noreturn));
static void Terminate (void) __attribute__((noreturn));
#ifndef HAVE_STRSIGNAL
static const char* strsignal (int sig);
#endif

//----------------------------------------------------------------------

/// Called when a signal is received.
static void OnSignal (int sig, siginfo*, void*)
{
    try {
	CApplication* pApp = CApplication::Instance();
	if (!pApp)
	    cerr << "[S] Fatal error: " << strsignal(sig) << endl;
	else if (pApp->OnSignal (sig))
	    return;
    } catch (exception& e) {
	cerr << "[S] Error: " << e << endl;
    } catch (...) {
	cerr << "[S] Unexpected error has occured." << endl;
    }
    exit (sig);
}

/// Called by the framework on unrecoverable exception handling errors.
static void Terminate (void)
{
    assert (!"Unrecoverable exception handling error detected.");
    raise (SIGABRT);
    exit (EXIT_FAILURE);
}

/// Called when an exception violates a throw specification.
static void OnUnexpected (void)
{
    cerr << "Unexpected exception caught. This is an internal error. Aborting." << endl;
    Terminate();
}

/// Installs OnSignal as handler for signals.
extern "C" void InstallCleanupHandlers (void)
{
    static const int8_t c_Signals[] = {
	SIGABRT, SIGALRM, SIGBUS, SIGCHLD, SIGFPE, SIGHUP, SIGILL, SIGINT,
	SIGIO, SIGIOT, SIGPIPE, SIGPOLL, SIGPWR, SIGQUIT, SIGSEGV, SIGSYS,
	SIGTERM, SIGTRAP, SIGTSTP, SIGTTIN, SIGTTOU, SIGUSR1, SIGUSR2,
	SIGVTALRM, SIGXCPU, SIGXFSZ
    };
    struct sigaction sa;
    sigemptyset (&sa.sa_mask);
    sa.sa_sigaction = OnSignal;
    sa.sa_flags = SA_RESTART | SA_SIGINFO;
    for (uoff_t i = 0; i < VectorSize(c_Signals); ++ i)
	sigaction (c_Signals[i], &sa, NULL);
    std::set_terminate (Terminate);
    std::set_unexpected (OnUnexpected);
}

/// Removes all environment strings and resets the needed ones to safe values.
extern "C" void ClearEnvironment (void)
{
    if (clearenv())
	throw libc_exception ("clearenv");
}

#ifndef HAVE_STRSIGNAL
/// Returns the textual description of a system signal \p sig.
static const char* strsignal (int sig)
{
    static const char* c_Signals[] = {
	"Invalid signal",
	"Terminal close command",
	"Interrupt",
	"Quit command",
	"Illegal operation",
	"I/O trap",
	"Bus error",
	"Floating point exception",
	"Kill command",
	"Console switch acknowledgement",
	"Segmentation fault",
	"Console release request",
	"Broken pipe",
	"Unhandled alarm",
	"Termination signal",
	"Stack fault error",
	"Child process message",
	"Unknown system error"
    };
    return (c_Signals [min (uoff_t(sig), VectorSize(c_Signals) - 1)]);
}
#endif

} // namespace fbgl

