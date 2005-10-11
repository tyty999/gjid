// palette.h
//
//	Defines VGA palette class.
//

#ifndef PALETTE_H_736264782AE3F934764E322922DB7EB3
#define PALETTE_H_736264782AE3F934764E322922DB7EB3

#include <fbgl.h>
using namespace fbgl;

#define MAXCOL               	256
#define MAXBRIGHT		64

class PaletteType : public CPalette {
public:
			PaletteType (void);
    void		SetAll (const ray_t* new_palette);
    void		FadeIn (color_t first = 0, color_t last = MAXCOL);
    void		FadeOut (color_t first = 0, color_t last = MAXCOL);
private:
    static CPalette	s_FadeBuffer;
};

#endif

