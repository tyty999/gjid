/* label.cc
**
**	Implements a label window.
*/

#include <game/label.h>
#include <string.h>

Label :: Label 
(int left, int top, int right, int bottom, char * ALabel, WORD AHandle)
: Win (left, top, right, bottom, AHandle)
{
    SetLabel (ALabel);
}

void Label :: Draw (void)
{
int tx, ty;

    mouse.Hide();
    PushGC();
    SetClipWindow (area);
    tx = area.x1 + area.width / 2 - (strlen (Text) * (font.Width() + 1)) / 2;
    ty = area.y1 + area.height / 2 - font.Height() / 2 - 1;
    FillRect (area, color [NormalBackColor]);
    font.PrintString (tx, ty, Text, color [NormalFrontColor]);
    PopGC();
    mouse.RescanBackgr();
    mouse.Show();
}

void Label :: SetLabel (char * NewLabel)
{
    if (strlen (NewLabel) > 0) {
       strncpy (Text, NewLabel, min (strlen (NewLabel), (size_t)MAX_LABEL + 1));
       Text [strlen (NewLabel)] = 0;
    }
}

