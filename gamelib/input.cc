/* input.cc
**
**	Implements a Win-based text input box.
*/

#include <game/input.h>
#include <string.h>
#include <ctype.h>

InputBox :: InputBox
(int left, int top, int right, int bottom, 
 char * APrompt, WORD AMaxChar, WORD AHandle)
: Button (left, top, right, bottom, APrompt, AHandle)
{
    MaxChar = AMaxChar;
    Buffer = new char [MaxChar];
    memset (Buffer, '\x0', MaxChar * sizeof(char));
    cPos = 0;
}

void InputBox :: Draw (void)
{
WORD ibo;

    mouse.Hide();
    PushGC();
    SetClipWindow (area.x1, area.y1, area.x2, area.y2);

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
       ibo = font.PrintString (area.x1 + 5, area.y1 + 2, Text, 
       			       color [DisabledFrontColor]);
       font.PrintString (area.x1 + 10 + ibo, area.y1 + 2, Buffer, 
       			 color [DisabledFrontColor]);
    }
    else if (Active) {
       FillRect (area, color [ActiveBackColor]);
       Line (area.x1, area.y2 - 1, area.x2 - 1, area.y2 - 1, 
             color [ActiveDarkBorderColor]);
       Line (area.x2 - 1, area.y1, area.x2 - 1, area.y2 - 1, 
             color [ActiveDarkBorderColor]);
       Line (area.x1, area.y1, area.x2 - 2, area.y1, 
             color [ActiveLightBorderColor]);
       Line (area.x1, area.y1, area.x1, area.y2 - 1, 
             color [ActiveLightBorderColor]);
       ibo = font.PrintString (area.x1 + 6, area.y1 + 3, Text, 
       			       color [ActiveFrontColor]);
       font.PrintString (area.x1 + 11 + ibo, area.y1 + 3, Buffer, 
       			 color [ActiveFrontColor]);
    }
    else {
       FillRect (area, color [NormalBackColor]);
       Line (area.x1, area.y2 - 1, area.x2 - 1, area.y2 - 1, 
             color [NormalDarkBorderColor]);
       Line (area.x2 - 1, area.y1, area.x2 - 1, area.y2 - 1, 
             color [NormalDarkBorderColor]);
       Line (area.x1, area.y1, area.x2 - 2, area.y1, 
             color [NormalLightBorderColor]);
       Line (area.x1, area.y1, area.x1, area.y2 - 1, 
             color [NormalLightBorderColor]);
       ibo = font.PrintString (area.x1 + 5, area.y1 + 2, Text, 
       			       color [NormalFrontColor]);
       font.PrintString (area.x1 + 11 + ibo, area.y1 + 2, Buffer, 
       			 color [NormalFrontColor]);
    }
    PopGC();
    mouse.RescanBackgr();
    mouse.Show();
}

HandlerStatus InputBox :: MousePressEvent (int x, int y, ButtonType button)
{
    if (!Disabled && !Pressed && UserHandler != NULL) {
       UserHandler (Buffer);
       Pressed = TRUE;
    }
    return (HandlerUsedEvent);
}

HandlerStatus InputBox :: KeyPressEvent (int key)
{
    if (key == KEY_BACKSPACE && cPos > 0) {
       --cPos;
       Buffer [cPos] = '\x0';
    }
    else if (key == KEY_ENTER && UserHandler != NULL)
       UserHandler (Buffer);
    else if (isprint(key) && cPos < MaxChar) {
       Buffer [cPos] = (char) key;
       ++ cPos;
    }
    else
       Buffer [cPos] = '\x0';

    Draw();
    return (HandlerUsedEvent);
}

void InputBox :: SetDefault (char * NewString)
{
    memset (Buffer, 0, MaxChar);
    strncpy (Buffer, NewString, min (strlen (NewString), (size_t)MaxChar));
    Buffer [strlen (NewString)] = 0;
    cPos = strlen (NewString);
}

InputBox :: ~InputBox (void)
{
    if (Buffer != NULL)
       delete [] Buffer;
}

