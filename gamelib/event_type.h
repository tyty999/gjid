/* event_type.h
**
**	This is a separate declaration for the EventType.
*/

#ifndef __EVENT_TYPE_H
#define __EVENT_TYPE_H

#include <mdefs.h>
#include "mouse.h"

typedef enum {
   Nothing	= 0x00,
   MouseMoved	= 0x01,
   MousePressed	= 0x02,
   MouseReleased= 0x04,
   KeyPressed	= 0x08,
   DisableWindow= 0x10,
   EnableWindow	= 0x20,
   RefreshWindow= 0x40,
   QuitMainLoop	= 0x80
} EventKind;			

class EventType {
public:
    EventKind		Type;
    WORD		Dest;

    int			xpos;
    int 		ypos;
    ButtonType		button;

    WORD		Key;
    WORD		LData;
};

#endif

