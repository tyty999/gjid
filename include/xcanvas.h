/* xcanvas.h
**
**	Defines a simple interface to XLib. Performs most of the needed chores.
** Your program should be linked with -lX11.
** Typically you would create your own class derived from this one and 
** overload some functions, mainly the *Handler functions.
**
** All the drawing is best performed in the ConfigureNotifyHandler function and
** in the IdleAction.
**
** includes:	    
**	<mdefs.h>	- for BOOL
**	<xwindow.h>	- base class
*/	 				    

#ifndef __XCANVAS_H
#define __XCANVAS_H
				    
#include <mdefs.h>
#include <xwindow.h>

#define MAXCOLORS	256

class XCanvas : public XWindow {
protected:
     GC			gc;
     Colormap		cmap;	
     Pixmap		pmap;	// If buffering is defined, this is the buffer
     Drawable		d;	// Points to where drawing is going
     BOOL		buffer;
public:
			XCanvas (int x = DEF_X, int y = DEF_Y, int w = DEF_W, int h = DEF_H);

     void		SetColorCell (int color, int r, int g, int b);
     void		SetColorPalette (int * RGBTriples);
     virtual void	SetForeground (int color);
     virtual void	SetBackground (int color);

     virtual void	DrawPixel (int x, int y);
     virtual void	DrawLine (int x1, int y1, int x2, int y2);
     virtual void	DrawRectangle (int x1, int y1, int x2, int y2);     
     virtual void	FillRectangle (int x1, int y1, int x2, int y2);
     virtual void       DrawPolygon (XPoint * points, int nPts, int type = Complex);
     virtual void	FillPolygon (XPoint * points, int nPts, int type = Complex);

     void		FlushBuffer (void);
     virtual void       EnableBuffer (void);
     virtual void       DisableBuffer (void);

     virtual WinStat	ConfigureNotifyHandler (XConfigureEvent* event);
     virtual WinStat	VisibilityNotifyHandler (void);

     virtual	       ~XCanvas (void);	
};

#endif

