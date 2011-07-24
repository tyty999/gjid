// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#define _GNU_SOURCE 1 // for strsignal (optional)
#include "app.h"
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

//----------------------------------------------------------------------

static void OnUnexpected (void) __attribute__((noreturn));
static void Terminate (void) __attribute__((noreturn));
#ifndef HAVE_STRSIGNAL
const char* strsignal (int sig);
#endif

//----------------------------------------------------------------------

/// Called when a signal is received.
static void OnSignal (int sig)
{
    try {
	CApplication* pApp = CApplication::Instance();
	if (!pApp)
	    cerr.format ("[S] Fatal error: %s\n", strsignal(sig));
	else if (pApp->OnSignal (sig))
	    return;
    } catch (exception& e) {
	cerr << "[S] Error: " << e << endl;
    } catch (...) {
	cerr.format ("[S] Unexpected error has occured.\n");
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
    cerr.format ("Unexpected exception fatal internal error occured.\n");
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
    for (uoff_t i = 0; i < VectorSize(c_Signals); ++ i)
	signal (c_Signals[i], OnSignal);
    std::set_terminate (Terminate);
    std::set_unexpected (OnUnexpected);
}
