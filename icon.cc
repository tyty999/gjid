/* icon.cc
**
** 	Implements the Icon class
**
** includes:
**	icon.h		- for WORD, BYTE
*/

#include <game/icon.h>
#include <string.h>

Icon :: Icon (WORD Width, WORD Height, BYTE * NewBits)
{			 
    width = Width;
    height = Height;
    bits = new BYTE [width * height];
    if (NewBits != NULL)
       memcpy (bits, NewBits, width * height);
}

void Icon :: SetImage (WORD Width, WORD Height, BYTE * NewBits)
{
    width = Width;
    height = Height;
    if (bits != NULL)
       delete [] bits;
    bits = new BYTE [width * height];
    if (NewBits != NULL)
       memcpy (bits, NewBits, width * height);
}

Icon& Icon :: operator= (const Icon& ToBe)
{
    width = ToBe.width;
    height = ToBe.height;
    if (bits != NULL)
       delete [] bits;
    bits = new BYTE [width * height];
    memcpy (bits, ToBe.bits, width * height);
    return (*this);
}

void Icon :: BlendWith (const Icon& AnIcon, BlendType how)
{
    if (width != AnIcon.width || height != AnIcon.height)
       return;

    if (how == SeeThroughBlend) {
       for (WORD i = 0; i < width * height; ++ i) {
	  if (bits[i] == 0)
	     bits[i] = AnIcon.bits[i];
       }
    }
    else if (how == AndBlend) {
       for (WORD i = 0; i < width * height; ++ i)
	  bits[i] &= AnIcon.bits[i];
    }
    else if (how == XorBlend) {
       for (WORD i = 0; i < width * height; ++ i)
	  bits[i] ^= AnIcon.bits[i];
    }
    else if (how == OrBlend) {
       for (WORD i = 0; i < width * height; ++ i)
	  bits[i] |= AnIcon.bits[i];
    }
}

void Icon :: Read (ifstream& is)
{   
char IdStr [ICON_ID_STRING_LENGTH + 1];
		
    is.read (IdStr, sizeof(BYTE) * ICON_ID_STRING_LENGTH);
    
    if (strncmp (IdStr, ICON_ID_STRING, ICON_ID_STRING_LENGTH) == 0)
    {
       is.read ((char*) &width, sizeof(WORD));
       is.read ((char*) &height, sizeof(WORD));
       if (bits != NULL)
          delete [] bits;
       bits = new BYTE [width * height];
       is.read ((char*) bits, sizeof(BYTE) * width * height);
    }
    else {
       cout << "Icon: invalid icon file!\n";
       exit (1);
    }
}

void Icon :: Write (ofstream& os)
{   
char IdStr [ICON_ID_STRING_LENGTH + 1] = ICON_ID_STRING;
		
    os.write (IdStr, sizeof(BYTE) * ICON_ID_STRING_LENGTH);
    os.write ((const char*) &width, sizeof(WORD));
    os.write ((const char*) &height, sizeof(WORD));
    os.write ((const char*) bits, sizeof(BYTE) * width * height);
}

Icon :: ~Icon (void)
{	       
    if (bits != NULL)				      
       delete [] bits;
}

