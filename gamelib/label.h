/* label.h
**
**	Defines a label window.
*/

#ifndef __LABEL_H
#define __LABEL_H

#include "win.h"
#include "font.h"

#define MAX_LABEL		80

class Label : public Win {
protected:
    char			Text [MAX_LABEL];

public:
				Label (int x1, int y1, int x2, int y2, 
					char * ALabel, WORD AHandle);
    virtual void		Draw (void);
    virtual void		SetLabel (char * NewLabel);
};

#endif

