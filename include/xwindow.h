/* xwindow.h
**
**	Defines a simple interface to XLib. Performs most of the needed chores.
** Your program should be linked with -lX11.
** Typically you would create your own class derived from this one and 
** overload some functions, mainly the *Handler functions.
**
** includes:
**	<X11/Xlib.h>	- general Xlib routines.
*/	 				    

#ifndef __XWINDOW_H
#define __XWINDOW_H

#include <X11/Xlib.h>

#define DEF_X		0
#define DEF_Y		0
#define DEF_W	      640
#define DEF_H	      480

#define DEFAULT_EVENT_MASK	KeyPressMask | ButtonPressMask | \
				StructureNotifyMask | ExposureMask | \
				VisibilityChangeMask

// Define WinStat values
#define Idle		0
#define Exit		1

typedef	unsigned int	WinStat;

class XWindow {
protected:
    Display *		dpy;
    int			scr;
    Window		root;
    Window		win;
    int			xpos;
    int			ypos;
    int			width;
    int			height;
public:
			XWindow (int x = DEF_X, int y = DEF_Y, int w = DEF_W, int h = DEF_H);
    virtual void	Resize (int newWidth, int newHeight);
    virtual void  	MoveTo (int newX, int newY);
    virtual void	Surface (void);
    virtual void	Sink (void);

    virtual WinStat	EventHandler (XEvent * event);
    virtual WinStat	IdleAction (void) {return (Idle);};

    virtual WinStat	ConfigureNotifyHandler (XConfigureEvent * event);
    virtual WinStat	VisibilityNotifyHandler (void);
    virtual WinStat	MappingNotifyHandler (XMappingEvent * event);
    virtual WinStat	ButtonPressHandler (XButtonPressedEvent * event);
    virtual WinStat	KeyPressHandler (char key);

    virtual void	Run (void);
    virtual	       ~XWindow (void);
};

#endif

