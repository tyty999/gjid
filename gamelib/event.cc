/* event.cc
**
**	Implements event handler.
*/

#include <time.h>
#include <game/event.h>
#include <game/graph.h>
#include <queue.h>
#include <llist.h>

Queue<EventType> EventQueue;
LList<Win> WinList;

EventType * GetEvent (void)
{
static int oldx = 0,oldy = 0;
static ButtonType oldbutton = NoButtons;
static int mouseDelay = 0, oldTime = 0;
typedef EventKind	EK;	// For shorter manual casting
EventType * NewEvent;

    NewEvent = new EventType;

    mouseDelay += (clock() - oldTime);
    mouse.Update();

    NewEvent->Dest = HWorld;
    NewEvent->Type = Nothing;

    if ((NewEvent->Key = GetKey()) != 0)
       NewEvent->Type = (EK)(NewEvent->Type | KeyPressed);

    NewEvent->xpos = mouse.x();
    NewEvent->ypos = mouse.y();
    NewEvent->button = mouse.ButtonStatus();

    if (NewEvent->xpos != oldx || NewEvent->ypos != oldy)
    {
       NewEvent->Type = (EK)(NewEvent->Type | MouseMoved);
       oldx = NewEvent->xpos;
       oldy = NewEvent->ypos;
    }

    if (NewEvent->button == NoButtons && oldbutton != NoButtons)
       NewEvent->Type = (EK)(NewEvent->Type | MouseReleased);
    else if (NewEvent->button != NoButtons && 
             (oldbutton == NoButtons || mouseDelay > int(mouse.GetRepeatDelay()))) {
       NewEvent->Type = (EK)(NewEvent->Type | MousePressed);
       mouseDelay = 0;
    }
    oldbutton = NewEvent->button;

    if (NewEvent->Type != Nothing || EventQueue.IsEmpty())
       EventQueue.Append (NewEvent);
    else
       delete NewEvent;

    return (EventQueue.Serve());
}

EventType * WaitEvent (void)
{
EventType * NewEvent = NULL;

    do {
	if (NewEvent != NULL) {
	   delete NewEvent;
	   NewEvent = NULL;
	}
	NewEvent = GetEvent();
	if (NewEvent == NULL)
	   continue;
    }
    while (NewEvent->Type == Nothing);
    return (NewEvent);
}

void CreateEvent (WORD EType, WORD EDest, WORD EData)
{
EventType * NewEvent;
    NewEvent = new EventType;
    NewEvent->Type = (EventKind) EType;
    NewEvent->Dest = EDest;
    NewEvent->LData = EData;
    EventQueue.Append (NewEvent);
}

void FlushEvents (void)
{
    while (!EventQueue.IsEmpty())
        delete (EventQueue.Serve());
}

void ManageWindow (Win * AWin)
{
    WinList.Head (1);
    WinList.InsertBefore (AWin, 1);
}

void ReleaseWindow (Win * AWin)
{
    WinList.Head (1);
    while (WinList.OnList(1) && WinList.LookAt(1) != AWin)
       WinList.Next(1);

    if (WinList.OnList(1))	// If on list, must be == AWin
       WinList.Remove(1);
}

void RefreshDesktop (void)
{
    mouse.Hide();
    ClearScreen (0);
    mouse.RescanBackgr();
    mouse.Show();
    WinList.Head (1);
    while (WinList.OnList (1)) {
       WinList.LookAt (1)->Draw();
       WinList.Next (1);
    }
}

void MainLoop (void)
{
EventType * event;
BOOL Done = FALSE;

    FlushEvents();
    EnableClipping();
    RefreshDesktop();

    while (!Done) {		// Exit on quit event or no windows
       event = WaitEvent();
       if (event == NULL) {
	  cout << "NULL event\n";
	  exit (1);
       }
       if (event->Type == QuitMainLoop || WinList.IsEmpty())
	  Done = TRUE;
       else {
	  // Give the event to every available window until one takes it
	  //	or no more windows.
	  WinList.Head();
	  while (WinList.OnList()) {
	     if (WinList.LookAt()->EventHandler (*event) == HandlerUsedEvent)
		break;
	     WinList.Next();
	  }
       }
       if (event != NULL)
          delete event;	// event was created in CreateEvent or GetEvent
    }
    FlushEvents();
}

