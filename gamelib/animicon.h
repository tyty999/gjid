/* animicon.h
**
** 	An animicon basically look like a torus.
** Imagine a circularly linked list of Icons. This is called an Animloop
** Imagine a circularly linked list of Animloops. This is an Animicon!
** 
** The usual utilization will select an Animloop and roll it with Advance.
**
** includes:	  
**	mdefs.h		- for WORD, BYTE 
**	icon.h		- for Icon
**	lclist.h	- for circular linked list
*/

#ifndef __ANIMICON_H
#define __ANIMICON_H				  

#include <mdefs.h>
#include "icon.h"
#include <lclist.h>
						  
// Animicon data file starts with the following string
#define ANIMICON_ID_STRING			"ANIMICON"
#define ANIMICON_ID_STRING_LENGTH		8

// The linked list window that keeps the current frame position
//	this should be different from the default list window
//	to prevent erasure by abstract Read and Write functions.
#define CFWIN					3

class AnimIcon : public Streamable {
private:
    BOOL			dirty;	// Has the image changed since update?

protected:					    
    LCList< LCList<Icon> >	frames;		   

public:
    virtual void		Read (ifstream& is);
    virtual void		Write (ofstream& os);
    
public:
    inline			AnimIcon (void);
    inline void 		Rewind (void);
    virtual inline void	       	Advance (void);
    inline void		       	SetLoop (WORD NewLoop);
    inline void 	   	Display (WORD x, WORD y);
    inline BOOL			IsDirty (void);
    virtual inline	       ~AnimIcon (void);
};
			       
///////////////////////////////////////////////////////////////////////////
////////////////////////// Inline functions ///////////////////////////////
				       
inline AnimIcon :: AnimIcon (void)
{
    dirty = TRUE;
}

inline void AnimIcon :: Rewind (void)
{
    (frames.LookAt (CFWIN))->Head(CFWIN);
    dirty = TRUE;
}

inline void AnimIcon :: Advance (void)
{
    (frames.LookAt(CFWIN))->Next(CFWIN);
    dirty = TRUE;
}

inline void AnimIcon :: SetLoop 
(WORD NewLoop)
{	  
WORD n;
    frames.Head(CFWIN);
    for (n = 0; n < NewLoop; ++ n)
       frames.Next(CFWIN);
    dirty = TRUE;
}

inline void AnimIcon :: Display 
(WORD x, WORD y)
{
    // Take the current icon from the current loop
    // and call its Put method.
    if (!frames.IsEmpty())
       frames.LookAt(CFWIN)->LookAt(CFWIN)->Put (x, y);
    dirty = FALSE;
}		     

inline BOOL AnimIcon :: IsDirty (void)
{
    return (dirty);
}

inline AnimIcon :: ~AnimIcon (void)
{
    while (!frames.IsEmpty()) {
       frames.Head();
       frames.Delete();
    }
}

#endif

