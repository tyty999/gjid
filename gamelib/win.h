/* win.h
**
**	Defines simple window class.
*/

#ifndef __WIN_H
#define __WIN_H

#include <mdefs.h>
#include "graph.h"
#include "mouse.h"
#include "event_type.h"

// Define window color IDs for use by application programs
typedef enum {
    NormalBackColor, 
    NormalFrontColor, 
    NormalDarkBorderColor, 
    NormalLightBorderColor,
    ActiveBackColor, 
    ActiveFrontColor, 
    ActiveDarkBorderColor, 
    ActiveLightBorderColor,
    DisabledBackColor, 
    DisabledFrontColor, 
    DisabledDarkBorderColor, 
    DisabledLightBorderColor,
    ShadowColor,
    UserDefinedColor, 
    N_COLOR_TYPES
} WindowColor;

// The following type is returned by the event handler
typedef enum {
    HandlerUsedEvent,
    HandlerIgnoredEvent
} HandlerStatus;

// The following is the type of the user handler
typedef HandlerStatus	(*UserHandlerPtr)(...);

// Handle uniquely identifies a window (like in MSW)
typedef WORD		HandleType;
const HandleType	HWorld = 0;

class Win {
protected:
    Rectangle			area;
    ColorID                    	color [N_COLOR_TYPES];

protected:
    HandleType			Handle;
    BOOL			Active;
    BOOL			Disabled;
    BOOL			HasShadow;
    UserHandlerPtr		UserHandler;

public:
			     	Win (int x1, int y1, int x2, int y2, 
					HandleType AHandle);
    virtual void		Draw (void);
    virtual HandlerStatus	EventHandler (const EventType& event);
    virtual void		Move (int NewX, int NewY);
    virtual void		Resize (WORD NewLength, WORD NewWidth);
    virtual inline BOOL		IsInside (int x, int y) const;
    virtual inline void		SetColor (WindowColor what, ColorID to);
    virtual inline void		SetUserHandler (UserHandlerPtr NewHandler);

    virtual inline void			Enable (void);
    virtual inline void			Disable (void);
    virtual inline void			Activate (void);
    virtual inline void			Deactivate (void);
    virtual inline HandlerStatus	KeyPressEvent (int key);
    virtual inline HandlerStatus	MouseMoveEvent (int x, int y);
    virtual inline HandlerStatus	MousePressEvent (int x, int y, 
    						 ButtonType button);
    virtual inline HandlerStatus	MouseReleaseEvent (int x, int y, 
    						   ButtonType button);
    virtual inline void			IdleEvent (void);

    virtual		       ~Win (void);
};

///////////////////////////////////////////////////////////////////////////////

inline void Win :: IdleEvent (void)
{
}

inline BOOL Win :: IsInside (int x, int y) const
{
    if (x >= area.x1 && x < area.x2 && y >= area.y1 && y < area.y2)
       return (TRUE);
    return (FALSE);
}

inline void Win :: Enable (void)
{
    Disabled = FALSE;
}

inline void Win :: Disable (void)
{
    Disabled = TRUE;
}

inline void Win :: Activate (void)
{
    Active = TRUE;
}

inline void Win :: Deactivate (void)
{
    Active = FALSE;
}

inline HandlerStatus Win :: MouseMoveEvent (int, int)
{
    return (HandlerIgnoredEvent);
}

inline HandlerStatus Win :: MousePressEvent (int, int, ButtonType)
{
    return (HandlerIgnoredEvent);
}

inline HandlerStatus Win :: MouseReleaseEvent (int, int, ButtonType)
{
    return (HandlerIgnoredEvent);
}

inline HandlerStatus Win :: KeyPressEvent (int)
{
    return (HandlerIgnoredEvent);
}

inline void Win :: SetUserHandler (UserHandlerPtr NewHandler)
{
    UserHandler = NewHandler;
}

inline void Win :: SetColor (WindowColor what, ColorID to)
{
    color [what] = to;
}

#endif

