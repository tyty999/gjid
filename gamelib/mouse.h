/* mouse.h
**
**	Defines a mouse class based on Mob
*/

#ifndef __MOUSE_H
#define __MOUSE_H

#include <time.h>
#include "mob.h"

#define DEFAULT_MOUSE_REP_DELAY   (CLOCKS_PER_SEC / 3)

typedef enum {
    NoButtons    = 0x00,
    LeftButton   = 0x01,
    RightButton  = 0x02,
    Reset        = 0xff,
    EitherButton = 0x03
} ButtonType;

class Mouse : public Mob {
private:
    clock_t			LastClick;

protected:
    BYTE			nbutton;
    BYTE			obutton;
    WORD			RepeatDelay;

protected:
    void			DefaultCursor (void);

public:
				Mouse (void);
    void			MoveTo (WORD x, WORD y);
    ButtonType			ButtonStatus (void);
    void			Update (void);
    inline void			SetRepeatDelay (WORD NewDelay);
    inline WORD			GetRepeatDelay (void);
    BOOL			IsPressed (ButtonType myButtons);
    BOOL			IsClicked (ButtonType myButtons);
    BOOL			IsInBox (int x1, int y1, int x2, int y2);
			       ~Mouse (void);
};

/////////////////////////////////////////

inline void Mouse :: SetRepeatDelay (WORD NewDelay)
{
    RepeatDelay = NewDelay;
}

inline WORD Mouse :: GetRepeatDelay (void)
{
    return (RepeatDelay);
}

/////////////////////////////////////////

extern Mouse mouse;

#endif

