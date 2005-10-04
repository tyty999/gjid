// app.cc
//

#include "app.h"
#include "con/fbdrv.h"
#include "con/constate.h"
#include "x11/xfb.h"

namespace fbgl {

//----------------------------------------------------------------------

CApplication* CApplication::s_pApp = NULL;

//----------------------------------------------------------------------

/// Default constructor.
CApplication::CApplication (void)
: CEventProcessor (),
  m_pFb (NULL),
  m_GC (),
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
    OnDraw (m_GC);
    if (m_pFb)
	m_pFb->Flush (m_GC);
}

void CApplication::OnCreate (void)
{
    m_pFb->Open();
    m_GC.link (m_pFb->Pixels(), m_pFb->Size());
    m_pFb->SetMode (m_pFb->FindClosestMode (640, 480, 60), 8);
}

void CApplication::OnDestroy (void)
{
    m_GC.unlink();
    m_pFb->Close();
}

bool CApplication::OnSignal (int)
{
    return (false);
}

void CApplication::OnKey (key_t key, keystate_t ks)
{
    CEventProcessor::OnKey (key, ks);
    if (key == key_F10 || (key == '\\' && ks[ks_Ctrl]))
	Quit();
}

void CApplication::OnIdle (void) {}
void CApplication::OnQuit (void) { SetFlag (f_QuitRequested); }
void CApplication::OnDraw (CGC&) {}

} // namespace fbgl

