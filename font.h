/* font.h
**
**	Defines user VGA font.
*/

#ifndef __FONT_H
#define __FONT_H

#include <mdefs.h>
#include <streamable.h>

#define NOL			256      // Number of letters

class Font : public Streamable {
private:
    BYTE *		data;
    BYTE		width;
    BYTE		height;
    WORD		LetterSize;

protected:
    BYTE *		GetLetterStart (WORD letter);

public:
			Font (void);
    int			PrintCharacter (WORD x, WORD y, char ascii, WORD color);
    int			PrintString (WORD x, WORD y, char * string, WORD color);
    void		ActivatePoint (WORD letter, WORD xpos, WORD ypos);
    void		DeactivatePoint (WORD letter, WORD xpos, WORD ypos);
    void		TogglePoint (WORD letter, WORD xpos, WORD ypos);
    BOOL		ReadPoint (WORD letter, WORD xpos, WORD ypos);
    void		Resize (BYTE NewWidth, BYTE NewHeight);
    inline BYTE		Width (void);
    inline BYTE		Height (void);
    virtual void	Read (ifstream& is);
    virtual void	Write (ofstream& os) const;
    virtual	       ~Font (void);
};

//////////////////////////////////////////////////////
inline BYTE Font :: Width (void)
{
    return (width);
}

inline BYTE Font :: Height (void)
{
    return (height);
}

//////////////////////////////////////////////////////

extern Font font;

#endif

