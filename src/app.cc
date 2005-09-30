// app.cc
//

#include "app.h"
#include "con/fbdrv.h"

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
	m_pFb->Open();
	m_GC.link (m_pFb->Pixels(), m_pFb->Size());
	Update();
	OnCreate();
	SetFlag (f_QuitRequested, false);
	while (!Flag (f_QuitRequested)) {
	    m_pFb->CheckEvents (this);
	    OnIdle();
	}
    } catch (...) {
	OnDestroy();
	m_GC.unlink();
	m_pFb->Close();
	throw;
    }
    OnDestroy();
    m_GC.unlink();
    m_pFb->Close();
}

/// Gets an appropriate framebuffer pointer.
CFramebuffer* CApplication::GetFramebuffer (void) const
{
    return (&CConsoleFramebuffer::Instance());
}

/// Redraws the application.
void CApplication::Update (void)
{
    OnDraw (m_GC);
    if (m_pFb)
	m_pFb->Flush (m_GC);
}

void CApplication::OnCreate (void) {}
void CApplication::OnDestroy (void) {}
void CApplication::OnIdle (void) {}
void CApplication::OnQuit (void) { SetFlag (f_QuitRequested); }
void CApplication::OnDraw (CGC&) {}

} // namespace fbgl

