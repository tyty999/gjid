/* button.cc
**
**	Implements a pushbutton based on Win.
*/

#include <game/button.h>

Button :: Button 
(int left, int top, int right, int bottom, char * ALabel, WORD AHandle)
: Label (left, top, right, bottom, ALabel, AHandle)
{
    Pressed = FALSE;
    Continuous = FALSE;
}

void Button :: Draw (void)
{
int tx, ty;

    mouse.Hide();
    PushGC();
    SetClipWindow (area);

    tx = area.x1 + area.width / 2 - (strlen (Text) * (font.Width() + 1)) / 2;
    ty = area.y1 + area.height / 2 - font.Height() / 2 - 1;

    if (Disabled) {
       FillRect (area, color [DisabledBackColor]);
       Line (area.x1, area.y2 - 1, area.x2 - 1, area.y2 - 1, 
       	     color [DisabledDarkBorderColor]);
       Line (area.x2 - 1, area.y1, area.x2 - 1, area.y2 - 1, 
       	     color [DisabledDarkBorderColor]);
       Line (area.x1, area.y1, area.x2 - 2, area.y1, 
       	     color [DisabledLightBorderColor]);
       Line (area.x1, area.y1, area.x1, area.y2 - 1, 
             color [DisabledLightBorderColor]);
       font.PrintString (tx, ty, Text, color [DisabledFrontColor]);
    }
    else if (Pressed) {
       FillRect (area, color [ActiveBackColor]);
       Line (area.x1, area.y2 - 1, area.x2 - 1, area.y2 - 1, 
       	     color [ActiveLightBorderColor]);
       Line (area.x2 - 1, area.y1, area.x2 - 1, area.y2 - 1, 
             color [ActiveLightBorderColor]);
       Line (area.x1, area.y1, area.x2 - 2, area.y1, 
             color [ActiveDarkBorderColor]);
       Line (area.x1, area.y1, area.x1, area.y2 - 1, 
       	     color [ActiveDarkBorderColor]);
       font.PrintString (tx + 1, ty + 1, Text, color [ActiveFrontColor]);
    }
    else {
       FillRect (area.x1, area.y1, area.x2, area.y2, color [NormalBackColor]);
       Line (area.x1, area.y2 - 1, area.x2 - 1, area.y2 - 1, 
       	     color [NormalDarkBorderColor]);
       Line (area.x2 - 1, area.y1, area.x2 - 1, area.y2 - 1, 
             color [NormalDarkBorderColor]);
       Line (area.x1, area.y1, area.x2 - 2, area.y1, 
             color [NormalLightBorderColor]);
       Line (area.x1, area.y1, area.x1, area.y2 - 1, 
             color [NormalLightBorderColor]);
       font.PrintString (tx, ty, Text, color [NormalFrontColor]);
    }
    PopGC();
    mouse.RescanBackgr();
    mouse.Show();
}

HandlerStatus Button :: MousePressEvent (int, int, ButtonType)
{
HandlerStatus status = HandlerUsedEvent;

    if (!Disabled && (!Pressed || Continuous) && UserHandler != NULL) {
       status = (HandlerStatus) (status | UserHandler());
       Pressed = TRUE;
       Draw();
    }
    return (status);
}

HandlerStatus Button :: MouseReleaseEvent (int, int, ButtonType)
{
    Pressed = FALSE;
    Draw();
    return (HandlerUsedEvent);
}

void Button :: SetContinuous (BOOL NewPolicy)
{
    Continuous = NewPolicy;
}

void Button :: Enable (void)
{
    Disabled = FALSE;
    Draw();
}

void Button :: Disable (void)
{
    Pressed = FALSE;
    Disabled = TRUE;
    Draw();
}

void Button :: Activate (void)
{
    Active = TRUE;
    Draw();
}

void Button :: Deactivate (void)
{
    Active = FALSE;
    Pressed = FALSE;
    Draw();
}

