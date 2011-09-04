// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "app.h"
#include "fb.h"
#include <signal.h>
#include <stdio.h>

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

//----------------------------------------------------------------------

CApp::CApp (void)
:_fb()
,_gc()
,_wantQuit (false)
{
    static const int8_t c_Signals[] = {
	SIGILL, SIGABRT, SIGBUS,  SIGFPE,  SIGSEGV, SIGSYS, SIGALRM, SIGXCPU,
	SIGHUP, SIGINT,  SIGQUIT, SIGTERM, SIGCHLD, SIGXFSZ, SIGPWR, SIGPIPE
    };
    for (uoff_t i = 0; i < VectorSize(c_Signals); ++ i)
	signal (c_Signals[i], OnSignal);
    std::set_terminate (Terminate);

    _fb.Open();
}

int CApp::Run (void)
{
    Update();
    for (_wantQuit = false; !_wantQuit; OnIdle())
	_fb.CheckEvents (this);
    return (EXIT_SUCCESS);
}

void CApp::Update (void)
{
    if (!_fb.GC().begin())
	return;
    OnDraw (_fb.GC());
    _fb.Flush();
}
