/* win.cc
**
**	Implements a simple window.
*/

#include <game/win.h>
#include <game/graph.h>
#include <game/event.h>

Win :: Win (int left, int top, int right, int bottom, WORD AHandle)
{
ColorID DefaultColors [N_COLOR_TYPES] = {7,0,8,15, 7,0,8,15, 7,8,24,30, 0};

    memcpy (color, DefaultColors, N_COLOR_TYPES * sizeof(ColorID));
    area.Set (left, top, right, bottom);

    Handle = AHandle;
    Active = FALSE;
    Disabled = FALSE;
    HasShadow = FALSE;
}

void Win :: Draw (void)
{
    mouse.Hide();
    FillRect (area, color [NormalBackColor]);
    if (HasShadow) {
       FillRect (area.x2, area.y1 + 5, area.x2 + 5, area.y2 + 5, 
                 color [ShadowColor]);
       FillRect (area.x1 + 5, area.y2, area.x2 + 5, area.y2 + 5, 
                 color [ShadowColor]);
    }
    mouse.RescanBackgr();
    mouse.Show();
}

void Win :: Move (int NewXpos, int NewYpos)
{
    area.x2 += NewXpos - area.x1;
    area.y2 += NewYpos - area.y1;
    area.x1 = NewXpos;
    area.y1 = NewYpos;
}

void Win :: Resize (WORD NewWidth, WORD NewHeight)
{
    area.x2 += NewWidth - area.width;
    area.y2 += NewHeight - area.height;
    area.width = NewWidth;
    area.height = NewHeight;
}

HandlerStatus Win :: EventHandler (const EventType& event)
{
HandlerStatus status = HandlerIgnoredEvent;

    if (event.Dest == HWorld || event.Dest == Handle) {
       if (event.Type & DisableWindow)
	  Disable();
       else if (event.Type & EnableWindow)
	  Enable();

       if (event.Type & RefreshWindow)
          Draw();

       if (event.Type & MouseMoved) {
	  if (IsInside (event.xpos, event.ypos) && !Active && !Disabled) {
	     Activate();
	     status = HandlerUsedEvent;
          }
          else if (!IsInside (event.xpos, event.ypos) && Active)
	     Deactivate();
       }

       // Route the event to specific handlers, converting coords to local
       if (Active && !Disabled) {
	  if (event.Type & MouseMoved)
	     status = (HandlerStatus) (status | MouseMoveEvent (
	     			       event.xpos - area.x1,
				       event.ypos - area.y1));
	  if (event.Type & MousePressed)
	     status = (HandlerStatus) (status | MousePressEvent (
	     			       event.xpos - area.x1,
				       event.ypos - area.y1, event.button));
	  if (event.Type & MouseReleased)
	     status = (HandlerStatus) (status | MouseReleaseEvent (
	     			       event.xpos - area.x1, 
				       event.ypos - area.y1, 
				       event.button));
	  if (event.Type & KeyPressed)
	     status = (HandlerStatus) (status | KeyPressEvent (event.Key));
       }
    }
    IdleEvent();
    return (status);
}

Win :: ~Win (void)
{
}

