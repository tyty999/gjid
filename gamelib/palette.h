/* palette.h
**
**	Defines VGA palette class.
*/

#ifndef __PALETTE_H
#define __PALETTE_H

#include <mdefs.h>
#include <streamable.h>

#define MAXCOL               	256
#define MAXBRIGHT		64

class PaletteType : public Streamable {
protected:
    BYTE		colors [MAXCOL * 3];

public:
			PaletteType (void);
    void		SetRGB (int ncol, int red, int green, int blue);
    void		SetAll (BYTE *new_palette);
    void		FadeIn (WORD Start = 0, WORD End = MAXCOL);
    void		FadeOut (WORD Start = 0, WORD End = MAXCOL);
    void		RotateLeft (WORD Start = 0, WORD End = MAXCOL);
    void		RotateRight (WORD Start = 0, WORD End = MAXCOL);
    void		Reset (void);
    void		Standardize (void);
    virtual void	Read (ifstream& is);
    virtual void	Write (ofstream& os) const;
    virtual	       ~PaletteType (void);
};

#endif

