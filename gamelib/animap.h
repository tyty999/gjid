/* animap.h
**
**	Defines a map of animated squares.
**
** includes:
**	animicon.h	- each square is an animicon.
*/

#ifndef __ANIMAP_H
#define __ANIMAP_H

#include <matrix.h>
#include "animicon.h"

#define ANIMAP_ID_STRING	"ANIMAP"
#define ANIMAP_ID_STRING_LENGTH	6

class AniMap : public Matrix<WORD> {
private:
    WORD			nPictures;
    LList<AnimIcon> 		Pictures;
    struct {
        WORD x, y, w, h;
    }				ViewArea;
    struct {
        WORD x, y;
    }				TopLeft;
    struct {
        WORD x, y;
    }				SquareSize;

public:
    BOOL			Wrapping;

public:
    virtual void		Write (ofstream& os);
    virtual void		Read (ifstream& is);

public:
				AniMap (WORD width, WORD height);
    virtual void		SetDisplayArea (WORD x1, WORD y1, 
    						WORD x2, WORD y2);
    virtual void		Advance (void);
    virtual inline void		MoveTo (WORD x, WORD y);
    virtual void		Display (void);

    virtual WORD		AddPicture (AnimIcon * NewPic);
    virtual void		SetSquareSize (WORD width, WORD height);
    virtual inline void		SetSquare (WORD x, WORD y, WORD value);
    virtual inline WORD		GetSquare (WORD x, WORD y);
    virtual inline AnimIcon *	GetSquarePicture (WORD x, WORD y);
    virtual		       ~AniMap (void);
};

/////////////////// inlines /////////////////////////////////////////////

inline void AniMap :: MoveTo (WORD x, WORD y)
{
    if (x + ViewArea.w / SquareSize.x < Width())
       TopLeft.x = x;
    if (y + ViewArea.h / SquareSize.y < Height())
       TopLeft.y = y;
}

inline void AniMap :: SetSquare (WORD x, WORD y, WORD value)
{
    (Matrix<WORD>::operator[](y))[x] = value;
}

inline WORD AniMap :: GetSquare (WORD x, WORD y)
{
    if (Wrapping) {
       y = y % Height();
       x = x % Width();
    }
    return ((Matrix<WORD>::operator[](y))[x]);
}

inline AnimIcon * AniMap :: GetSquarePicture (WORD x, WORD y)
{
    return (Pictures [GetSquare (x, y)]);
}

#endif

