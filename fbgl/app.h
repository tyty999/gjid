// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "fb.h"

namespace fbgl {

/// \class CApplication app.h fbgl.h
///
/// \brief Base class for user applications.
///
class CApplication : protected CEventProcessor {
public:
    enum {
	f_QuitRequested,
	f_Last,
	f_Max = 32
    };
    typedef int			argc_t;
    typedef const char* const*	argv_t;
public:
    virtual void	ProcessArguments (argc_t argc, argv_t argv);
    void		MainLoop (void);
    void		Update (void);
    inline void		Quit (void)			{ OnQuit(); }
    virtual bool	OnSignal (int sig);
   static CApplication*	Instance (void);
protected:
			CApplication (void);
    virtual	       ~CApplication (void);
    inline virtual void	OnIdle (void)	{ }
    inline virtual void	OnDraw (CGC&)	{ }
    virtual void	OnQuit (void);
    virtual void	OnCreate (void);
    virtual void	OnDestroy (void);
    virtual void	OnKey (key_t key);
    inline bool		Flag (uoff_t i) const		{ return (m_Flags[i]); }
    inline void		SetFlag (uoff_t i, bool v=true)	{ m_Flags.set (i, v); }
    inline void		SetMode (EStdFbMode m = stdmode_320x240x8, size_t freq = 60) { m_pFb->SetStandardMode (m, freq); }
private:
    CFramebuffer*	GetFramebuffer (void) const;
private:
    CFramebuffer*	m_pFb;		///< Pointer to the framebuffer backend.
    CGC			m_GC;		///< GC for drawing onto the offscreen buffer.
    bitset<f_Max>	m_Flags;	///< See f_ constants for flag values.
   static CApplication*	s_pApp;		///< App pointer for Instance()
};

//----------------------------------------------------------------------

extern "C" void InstallCleanupHandlers (void);

template <typename AppClass>
inline int TFbglMain (int argc, const char* const* argv)
{
    int rv = EXIT_FAILURE;
    InstallCleanupHandlers();
    try {
	CApplication& rApp = AppClass::Instance();
	rApp.ProcessArguments (argc, argv);
	rApp.MainLoop();
	rv = EXIT_SUCCESS;
    } catch (exception& e) {
	cout.flush(); cerr << "Error: " << e << endl;
    } catch (...) {
	cout.flush(); cerr.format ("Unexpected fatal error has occured\n");
    }
    return (rv);
}

//----------------------------------------------------------------------

#define FbglMain(appClass)				\
    int main (int argc, const char* const* argv)	\
	{ return (TFbglMain<appClass> (argc, argv)); }

//----------------------------------------------------------------------

} // namespace fbgl
