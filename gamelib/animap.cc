/* animap.h
**
**	Implements a map of animated squares.
*/

#include <string.h>
#include <game/graph.h>
#include <game/animap.h>

void AniMap :: Write (ofstream& os)
{
char IdStr [ANIMAP_ID_STRING_LENGTH + 1] = ANIMAP_ID_STRING;
AnimIcon * ai;
int i;

    os.write (IdStr, sizeof(BYTE) * ANIMAP_ID_STRING_LENGTH);
    Matrix<WORD>::Write (os);

    os.write (&SquareSize.x, sizeof(WORD));
    os.write (&SquareSize.y, sizeof(WORD));

    os.write (&nPictures, sizeof(WORD));
    Pictures.Head();
    for (i = 0; i < nPictures; ++ i) {
       if ((ai = Pictures.LookAt()) != NULL)
          ai->Write (os);
       else {
	  cout << "Missing frame!\n";
	  exit (1);
       }
       Pictures.Next();
    }
}

void AniMap :: Read (ifstream& is)
{
char IdStr [ANIMAP_ID_STRING_LENGTH + 1];
AnimIcon * NewPic;
int i;

    is.read (IdStr, sizeof(BYTE) * ANIMAP_ID_STRING_LENGTH);
    if (strncmp (IdStr, ANIMAP_ID_STRING, ANIMAP_ID_STRING_LENGTH) != 0) {
       cerr << "Invalid animap file!\n";
       exit (1);
    }

    Matrix<WORD>::Read (is);

    is.read (&SquareSize.x, sizeof(WORD));
    is.read (&SquareSize.y, sizeof(WORD));

    is.read (&nPictures, sizeof(WORD));
    for (i = 0; i < nPictures; ++ i) {
       NewPic = new AnimIcon;
       NewPic->Read (is);
       Pictures.Tail();
       Pictures.InsertAfter (NewPic);
    }
}

AniMap :: AniMap (WORD width, WORD height)
: Matrix<WORD> (height, width)
{
    SquareSize.x = 1;
    SquareSize.y = 1;
    ViewArea.x = 0;
    ViewArea.y = 0;
    ViewArea.w = 0;
    ViewArea.h = 0;
    TopLeft.x = 0;
    TopLeft.y = 0;
    Wrapping = FALSE;
}

AniMap :: ~AniMap (void)
{
}

void AniMap :: SetSquareSize (WORD width, WORD height)
{
    SquareSize.x = width;
    SquareSize.y = height;
}

void AniMap :: SetDisplayArea (WORD x1, WORD y1, WORD x2, WORD y2)
{
    ViewArea.x = x1;
    ViewArea.y = y1;
    ViewArea.h = y2 - y1;
    ViewArea.w = x2 - x1;
}
    						
void AniMap :: Advance (void)
{
    Pictures.Head();
    while (Pictures.OnList()) {
       (Pictures.LookAt())->Advance();
       Pictures.Next();
    }
}

void AniMap :: Display (void)
{
int HorizSquares, VertSquares;
int x, y;
AnimIcon * Current;

    EnableClipping();
    SetClipWindow (ViewArea.x, ViewArea.y, 
    			  ViewArea.x + ViewArea.w, 
			  ViewArea.y + ViewArea.h);

    HorizSquares = (ViewArea.w / SquareSize.x) + 1;
    VertSquares = (ViewArea.h / SquareSize.y) + 1;

    for (y = 0; y < VertSquares; ++ y) {
       for (x = 0; x < HorizSquares; ++ x) {
	  Current = GetSquarePicture (x + TopLeft.x, y + TopLeft.y);
	  Current->Display (ViewArea.x + x * SquareSize.x,
	  		    ViewArea.y + y * SquareSize.y);
       }
    }
    DisableClipping();
}

WORD AniMap :: AddPicture (AnimIcon * NewPic)
{
WORD PicID;

    PicID = nPictures;
    Pictures.Tail();
    Pictures.InsertAfter (NewPic);
    ++ nPictures;
    return (PicID);
}

