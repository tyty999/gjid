/* mouse.cc
**
**	Implements mouse class based on mob.
*/

#include <time.h>
#include <game/mouse.h>
#include <game/graph.h>

Mouse mouse;

Mouse :: Mouse (void)
{
    RepeatDelay = DEFAULT_MOUSE_REP_DELAY;
    DefaultCursor();
}

void Mouse :: MoveTo (WORD x, WORD y)
{
    MouseSetPosition (x, y);
    Mob :: MoveTo (x,y);
}

void Mouse :: DefaultCursor (void)
{
BYTE arrow_res[25] = {0x0F,0x0F,0x0F,0x0F,0x0F,
		      0x0F,0x0F,0x0F,0x00,0x00,
		      0x0F,0x0F,0x0F,0x00,0x00,
		      0x0F,0x00,0x00,0x0F,0x00,
		      0x0F,0x00,0x00,0x00,0x0F};
Icon ArrowIcon (5, 5, arrow_res);
    SetImage (ArrowIcon);
}

ButtonType Mouse :: ButtonStatus (void)
{
    return ((ButtonType) nbutton);
}

BOOL Mouse :: IsPressed (ButtonType myButtons)
{
    return (myButtons & nbutton);
}

BOOL Mouse :: IsClicked (ButtonType myButtons)
{
    return ((myButtons & obutton) && !IsPressed (myButtons));
}

BOOL Mouse :: IsInBox (int x1, int y1, int x2, int y2)
{
    if (x1 <= nx && y1 <= ny && x2 > nx && y2 > ny)
       return (TRUE);
    else
       return (FALSE);
}

void Mouse :: Update (void)
{
BYTE but;
WORD NewX, NewY;

    MouseUpdate();

    obutton = nbutton;
    nbutton = NoButtons;

    NewX = MouseXpos();
    NewY = MouseYpos();

    if (clock() - LastClick > RepeatDelay) {
       but = MouseButton();
       if (but & MOUSE_LEFTBUTTON)
	  nbutton |= LeftButton;
       if (but & MOUSE_RIGHTBUTTON)
	  nbutton |= RightButton;
       LastClick = RepeatDelay;
    }

    Mob :: MoveTo (NewX, NewY);
}

Mouse :: ~Mouse()
{
    MouseClose();
}

