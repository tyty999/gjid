/* input.h
**
**	Defines Win-based text input box.
*/

#ifndef __INPUT_H
#define __INPUT_H

#include "button.h"

class InputBox : public Button {
protected:
    char *			Buffer;
    WORD			cPos;
    WORD			MaxChar;

public:
				InputBox (int x1, int y1, int x2, int y2, 
			     	       char * APrompt, WORD AMaxChar, 
				       WORD AHandle);
    virtual void		Draw (void);
    virtual HandlerStatus	MousePressEvent (int x,int y,ButtonType button);
    virtual HandlerStatus	KeyPressEvent (int key);
    void			SetDefault (char * NewString);
    virtual 		       ~InputBox (void);
};

#endif

