#include <string.h>
#include <game/animicon.h>

void AnimIcon :: Read (ifstream& is)
{
char IdStr [ANIMICON_ID_STRING_LENGTH + 1];
register WORD nLoops, nFrames, l, f;
LCList<Icon> * NewList;
Icon * NewIcon;
		
    is.read (IdStr, sizeof(BYTE) * ANIMICON_ID_STRING_LENGTH);
    
    if (strncmp (IdStr, ANIMICON_ID_STRING, ANIMICON_ID_STRING_LENGTH) != 0) {
       cout << "Icon: invalid icon file!\n";
       exit (1);
    }

    is.read (&nLoops, sizeof(WORD));
    NewList = new LCList<Icon>;
    for (l = 0; l < nLoops; ++ l) {
       is.read (&nFrames, sizeof(WORD));
       NewIcon = new Icon (0, 0);
       for (f = 0; f < nFrames; ++ f)
	  NewIcon->Read (is);
       NewList->Tail();
       NewList->InsertAfter (NewIcon);
    }	      
    frames.Tail();
    frames.InsertAfter (NewList);
    dirty = TRUE;

    SetLoop (0);
    Rewind();
}

void AnimIcon :: Write (ofstream& os)
{
char IdStr [ANIMICON_ID_STRING_LENGTH + 1] = ANIMICON_ID_STRING;
register WORD nLoops, nFrames, l, f;
LCList<Icon> * CurLoop;
Icon * CurIcon;

    nLoops = frames.Size();
		
    os.write (IdStr, sizeof(BYTE) * ANIMICON_ID_STRING_LENGTH);
    os.write (&nLoops, sizeof(WORD));
    
    frames.Head();
    for (l = 0; l < nLoops; ++ l) {
       if ((CurLoop = frames.LookAt()) == NULL) {
	  cout << "AnimIcon: missing loop!\n";
	  exit (1);
       }
       nFrames = CurLoop->Size();
       os.write (&nFrames, sizeof(WORD));
       CurLoop->Head();
       for (f = 0; f < nFrames; ++ f)
       {
          if ((CurIcon = CurLoop->LookAt()) == NULL) {
	     cout << "AnimIcon: missing frame!\n";
	     exit (1);
          }
	  CurIcon->Write (os);
	  CurLoop->Next();
       }	      	 
       frames.Next();
    }
}


