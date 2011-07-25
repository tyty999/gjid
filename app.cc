// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "app.h"
#include "fb.h"
#include <signal.h>
#include <stdio.h>

//----------------------------------------------------------------------

void CApp::MainLoop (void)
{
    Update();
    for (m_WantQuit = false; !m_WantQuit; OnIdle())
	m_Fb.CheckEvents (this);
}

void CApp::Update (void)
{
    OnDraw (m_Fb.GC());
    m_Fb.Flush();
}

//----------------------------------------------------------------------

/// Called when a signal is received.
static void OnSignal (int sig)
{
    printf ("[S] Fatal error: %s\n", strsignal(sig));
    exit (-sig);
}

/// Called by the framework on unrecoverable exception handling errors.
static void Terminate (void)
{
    printf ("[T] Unexpected fatal error\n");
    exit (EXIT_FAILURE);
}

/// Installs OnSignal as handler for signals.
extern "C" void InstallCleanupHandlers (void)
{
    static const int8_t c_Signals[] = {
	SIGILL, SIGABRT, SIGBUS,  SIGFPE,  SIGSEGV, SIGSYS, SIGALRM, SIGXCPU,
	SIGHUP, SIGINT,  SIGQUIT, SIGTERM, SIGCHLD, SIGXFSZ, SIGPWR, SIGPIPE
    };
    for (uoff_t i = 0; i < VectorSize(c_Signals); ++ i)
	signal (c_Signals[i], OnSignal);
    std::set_terminate (Terminate);
}
