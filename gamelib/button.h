/* button.h
**
**	Defines a pushbutton based on Win
*/

#ifndef __BUTTON_H
#define __BUTTON_H

#include "label.h"

class Button : public Label {
protected:
    BOOL			Pressed;
    BOOL			Continuous;	// Require mouse release?

public:
				Button (int left, int top, int right, 
					int bottom, char * ALabel, 
					WORD AHandle);
    virtual void		Draw (void);
    virtual void		Activate (void);
    virtual void		Deactivate (void);
    virtual void		Enable (void);
    virtual void		Disable (void);
    void			SetContinuous (BOOL NewPolicy);
    virtual HandlerStatus	MousePressEvent (int x,int y,ButtonType button);
    virtual HandlerStatus	MouseReleaseEvent (int x,int y,ButtonType button);
};

#endif

