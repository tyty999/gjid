// Copyright (c) 2003-2006 by Mike Sharov <msharov@users.sourceforge.net>
//
// state.h
//

#ifndef STATE_H_17C5725A6418797A3B7DD95B0A18BF4C
#define STATE_H_17C5725A6418797A3B7DD95B0A18BF4C

#include <utio.h>
#include "../fb.h"

#define SIG_VTACQ	SIGUSR2
#define SIG_VTREL	SIGUSR1

namespace fbgl {
using namespace ustl;

//----------------------------------------------------------------------

class CConsoleState {
public:
    static CConsoleState&	Instance (void);
    void			SetTerm (const char* termname = NULL);
    void			RegisterFramebuffer (CFramebuffer* pRoot);
    inline bool			IsActive (void) { return (m_bActive); }
    void			Activate (bool v = true);
    void			OnResize (void);
    int				VtIndex (void) const;
    void			EnterGraphicsMode (void);
    void			LeaveGraphicsMode (void);
    inline bool			DecodeKey (istream& is, wchar_t& kv, utio::CKeyboard::metastate_t& kf) const { return (m_Kb.DecodeKey (is, kv, kf)); }
    inline const utio::CKeyboard&	Keyboard (void) const	{ return (m_Kb); }
private:
				CConsoleState (void);
			       ~CConsoleState (void);
    void			AttachToConsole (void);
private:
    CFramebuffer*		m_pFramebuffer;	///< Pointer to the root window.
    utio::CTerminfo		m_TI;
    utio::CKeyboard		m_Kb;
    long int			m_OrigVtMode;	///< KD_TEXT or KD_GRAPHICS
    bool			m_bActive;	///< True if the console is currently active.
};

//----------------------------------------------------------------------

} // namespace fbgl

#endif

