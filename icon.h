/* icon.h
**
** 	Defines an Icon class
**
** includes:
**	streamable.h	- base class
**	mdefs.h		- for WORD, BYTE
*/

#ifndef __ICON_H
#define __ICON_H

#include <mdefs.h>
#include <streamable.h>	
#include "graph.h"

#define ICON_ID_STRING			"ICON"
#define ICON_ID_STRING_LENGTH		4		       

typedef enum {
    NoBlend,
    SeeThroughBlend,
    AndBlend,
    XorBlend,
    OrBlend,
    LastBlend
} BlendType;

class Icon : public Streamable {
protected:
    BYTE *			bits;		
    WORD			width;
    WORD			height;
    
public:
    virtual void		Read (ifstream& is);
    virtual void		Write (ofstream& os);
    
public:
				Icon (WORD Width = 1, WORD Height = 1, 
				      BYTE * NewBits = NULL);
    inline void			Put (int x, int y) const;
    inline void			Get (int x, int y);
    inline WORD			Width (void) const;
    inline WORD			Height (void) const;
    void			SetImage (WORD Width, WORD Height,
    					  BYTE * NewBits = NULL);

    inline void			SetPixel (WORD x, WORD y, BYTE color);
    inline BYTE			GetPixel (WORD x, WORD y) const;
    inline void			SetRow (WORD row, BYTE * dots);
    inline void			SetCol (WORD col, BYTE * dots);
    inline BYTE *		GetRow (WORD row);

    void			BlendWith (const Icon& AnIcon, BlendType how);
    Icon&			operator= (const Icon& ToBe);

    virtual	      	       ~Icon (void);
};	
		    
///////////////////////////////////////////////////////////////////////
/////////////////// Functions /////////////////////////////////////////

inline void Icon :: Put (int x, int y) const
{	
    PutImage (x, y, width, height, bits);
}

inline void Icon :: Get (int x, int y)
{	
    GetImage (x, y, width, height, bits);
}

inline WORD Icon :: Width (void) const
{	
    return (width);
}

inline WORD Icon :: Height (void) const
{	
    return (height);
}

inline void Icon :: SetPixel (WORD x, WORD y, BYTE color)
{	
    if (y < height && x < width)
       bits [y * width + x] = color;
}

inline BYTE Icon :: GetPixel (WORD x, WORD y) const
{	
    return (bits [y * width + x]);
}

inline BYTE * Icon :: GetRow (WORD row)
{	
    return (&bits [row * width]);
}

inline void Icon :: SetRow (WORD row, BYTE * dots)
{	
    if (row < height)
       memcpy (&bits [row * width], dots, width);
}

inline void Icon :: SetCol (WORD col, BYTE * dots)
{	
    if (col >= width)
       return;
    BYTE* bp = &bits [col];
    BYTE* dp = dots;
    for (WORD i = 0; i < height; ++ i) {
       *bp = *dp;
       ++ dp;
       bp += width;
    }
}

#endif

