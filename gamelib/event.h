/* event.h
**
**	Declares class for handling events.
*/

#ifndef __EVENT_H
#define __EVENT_H

#include "win.h"
#include "event_type.h"
#include "mouse.h"

/*---------------------------------------------------------------------------*/
 EventType *	GetEvent (void);
 EventType *	WaitEvent (void);
 void		CreateEvent (WORD EType, WORD EDest = HWorld, WORD EData = 0);
 void		FlushEvents (void);
 void		MainLoop (void);
 void		ManageWindow (Win * AWin);
 void		ReleaseWindow (Win * AWin);
 void		RefreshDesktop (void);
/*---------------------------------------------------------------------------*/

#endif

