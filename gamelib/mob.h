/* mob.h
**
**	Moving object definition.
*/

#ifndef __MOB_H
#define __MOB_H

#include "icon.h"

class Mob {
protected:
    WORD                      	ox;
    WORD                      	oy;
    WORD                      	nx;
    WORD                      	ny;
    Icon			image;
    Icon			backgr;
    Icon			projection;    // image + backgr
    BOOL			Hidden;

public:
    BlendType			Blending;

public:
				Mob (void);
    virtual void		RescanBackgr (void);
    virtual void		Show (void);
    virtual void		Hide (void);
    virtual void		SetImage (const Icon& NewImage);
    virtual void		MoveTo (WORD x, WORD y);
    inline int			x (void) { return (nx);};
    inline int			y (void) { return (ny);};
    virtual		       ~Mob (void);
};

#endif

