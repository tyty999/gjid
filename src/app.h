// app.h
//

#ifndef APP_H_0AEE96827DC77ABC0A46F062495765A9
#define APP_H_0AEE96827DC77ABC0A46F062495765A9

#include "fb.h"
#include "evp.h"
#include "gc.h"

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
protected:
			CApplication (void);
    virtual	       ~CApplication (void);
    virtual void	OnIdle (void);
    virtual void	OnDraw (CGC& gc);
    virtual void	OnQuit (void);
    virtual void	OnCreate (void);
    virtual void	OnDestroy (void);
    inline bool		Flag (uoff_t i) const		{ return (m_Flags[i]); }
    inline void		SetFlag (uoff_t i, bool v=true)	{ m_Flags.set (i, v); }
private:
    CFramebuffer*	GetFramebuffer (void) const;
private:
    CFramebuffer*	m_pFb;
    CGC			m_GC;
    bitset<f_Max>	m_Flags;
};

} // namespace fbgl

#endif

