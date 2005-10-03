// app.cc
//

#include "app.h"
#include "con/fbdrv.h"
#include "con/constate.h"

namespace fbgl {

/// Default constructor.
CApplication::CApplication (void)
: m_pFb (NULL),
  m_GC (),
  m_Flags (0)
{
}

/// Virtual destructor.
CApplication::~CApplication (void)
{
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
    const char* pDisp = getenv ("DISPLAYX");
    if (!pDisp)
	throw runtime_error ("this program requires the framebuffer console or an X server");
    return (NULL);
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
}

void CApplication::OnDestroy (void)
{
    m_GC.unlink();
    m_pFb->Close();
}

void CApplication::OnIdle (void) {}
void CApplication::OnQuit (void) { SetFlag (f_QuitRequested); }
void CApplication::OnDraw (CGC&) {}

} // namespace fbgl

