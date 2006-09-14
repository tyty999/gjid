// This file is part of the fbgl library.
//
// Copyright (C) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.
// 
// app.cc
//

#include "app.h"
#include "con/fb.h"
#include "con/state.h"
#include "x11/fb.h"

namespace fbgl {

//----------------------------------------------------------------------

CApplication* CApplication::s_pApp = NULL;

//----------------------------------------------------------------------

/// Default constructor.
CApplication::CApplication (void)
: CEventProcessor (),
  m_pFb (NULL),
  m_Flags (0)
{
    assert (!s_pApp && "CApplication derivatives must be singletons!");
    s_pApp = this;
}

/// Virtual destructor.
CApplication::~CApplication (void)
{
    assert (s_pApp == this && "CApplication derivatives must be singletons!");
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
    m_pFb = GetFramebuffer();
    try {
	OnCreate();
	Update();
	SetFlag (f_QuitRequested, false);
	while (!Flag (f_QuitRequested)) {
	    m_pFb->CheckEvents (this);
	    OnIdle();
	}
    } catch (...) {
	OnDestroy();
	throw;
    }
    OnDestroy();
}

/// Gets an appropriate framebuffer pointer.
CFramebuffer* CApplication::GetFramebuffer (void) const
{
    const int vti = CConsoleState::Instance().VtIndex();
    if (vti >= 0)
	return (&CConsoleFramebuffer::Instance());
    const char* pDisp = getenv ("DISPLAY");
    if (!pDisp)
	throw runtime_error ("this program requires the framebuffer console or an X server");
    return (&CXlibFramebuffer::Instance());
}

/// Redraws the application.
void CApplication::Update (void)
{
    if (m_pFb) {
	OnDraw (m_pFb->GC());
	m_pFb->Flush();
    }
}

void CApplication::OnCreate (void)
{
    m_pFb->Open();
}

void CApplication::OnDestroy (void)
{
    m_pFb->Close();
}

#ifndef HAVE_STRSIGNAL
const char* strsignal (int sig);	// In app.cc
#endif
bool CApplication::OnSignal (int sig)
{
    cerr.format ("Fatal error: %s\n", strsignal(sig));
    return (false);
}

void CApplication::OnKey (key_t key, keystate_t ks)
{
    CEventProcessor::OnKey (key, ks);
    if (key == key_F10 || (key == '\\' && ks[ks_Ctrl]))
	Quit();
}

void CApplication::OnQuit (void) { SetFlag (f_QuitRequested); }

} // namespace fbgl

