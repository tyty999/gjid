// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "fb.h"

/// Base class for application objects
class CApp {
public:
    typedef int			argc_t;
    typedef const char* const*	argv_t;
    typedef wchar_t		key_t;		///< Used for keycodes.
    typedef uint32_t		bidx_t;		///< Mouse button index.
public:
    inline virtual void		ProcessArguments (argc_t, argv_t) {}
    inline void			Quit (void)	{ OnQuit(); }
    void			Update (void);
    void			MainLoop (void);
protected:
    friend class CXlibFramebuffer;
    inline			CApp (void)	: m_Fb(), m_GC(), m_WantQuit (false) { m_Fb.Open(); }
    inline virtual void		OnIdle (void)	{ }
    inline virtual void		OnDraw (CGC&)	{ }
    inline virtual void		OnQuit (void)	{ m_WantQuit = true; }
    inline virtual void		OnKey (key_t)	{ }
    inline virtual void		OnMouseMove (coord_t, coord_t) {}
    inline virtual void		OnButtonDown (bidx_t, coord_t, coord_t) {}
    inline void			CreateWindow (const char* title, coord_t w, coord_t h) { m_Fb.CreateWindow (title, w, h); }
private:
    CXlibFramebuffer		m_Fb;		///< The framebuffer backend.
    CGC				m_GC;		///< GC for drawing onto the offscreen buffer.
    bool			m_WantQuit;	///< True if want to quit
};

//----------------------------------------------------------------------

extern "C" void InstallCleanupHandlers (void);

template <typename AppClass>
inline int TFbglMain (int argc, const char* const* argv)
{
    int rv = EXIT_FAILURE;
    InstallCleanupHandlers();
    try {
	AppClass& rApp = AppClass::Instance();
	rApp.AppClass::ProcessArguments (argc, argv);
	rApp.AppClass::MainLoop();
	rv = EXIT_SUCCESS;
    } catch (exception& e) {
	cout.flush(); cerr << "Error: " << e << endl;
    } catch (...) {
	cout.flush(); cerr << "Unexpected fatal error has occured\n";
    }
    return (rv);
}

//----------------------------------------------------------------------

#define FbglMain(appClass)				\
    int main (int argc, const char* const* argv)	\
	{ return (TFbglMain<appClass> (argc, argv)); }

//----------------------------------------------------------------------
