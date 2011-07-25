// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "app.h"
#include "fb.h"
#include <signal.h>
#include <stdio.h>

//----------------------------------------------------------------------

CApplication* CApplication::s_pApp = NULL;

//----------------------------------------------------------------------

/// Default constructor.
CApplication::CApplication (void)
: CEventProcessor ()
, m_Fb()
, m_GC()
, m_Flags (0)
{
    assert (!s_pApp && "CApplication derivatives must be singletons!");
    s_pApp = this;
}

/// Virtual destructor.
CApplication::~CApplication (void)
{
    assert (s_pApp == this && "CApplication derivatives must be singletons!");
    OnDestroy();
    s_pApp = NULL;
}

/*static*/ CApplication* CApplication::Instance (void)
{
    return (s_pApp);
}

/// Call to process arguments.
void CApplication::ProcessArguments (argc_t, argv_t)
{
}

/// Main event loop.
void CApplication::MainLoop (void)
{
    OnCreate();
    Update();
    SetFlag (f_QuitRequested, false);
    while (!Flag (f_QuitRequested)) {
	m_Fb.CheckEvents (this);
	OnIdle();
    }
}

/// Redraws the application.
void CApplication::Update (void)
{
    OnDraw (m_Fb.GC());
    m_Fb.Flush();
}

void CApplication::OnCreate (void)
{
    m_Fb.Open();
}

void CApplication::OnDestroy (void)
{
    m_Fb.Close();
}

bool CApplication::OnSignal (int sig)
{
    cerr.format ("Fatal error: %s\n", strsignal(sig));
    return (false);
}

void CApplication::OnKey (key_t key)
{
    CEventProcessor::OnKey (key);
    if (key == XK_F10 || key == ('\\'|XKM_Ctrl))
	Quit();
}

void CApplication::OnQuit (void) { SetFlag (f_QuitRequested); }

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
