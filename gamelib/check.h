
#ifndef __MCHECK_H
#define __MCHECK_H

#define MCHECK_ID            0x0107

typedef enum {On, Off, Disabled} StateType;

class MCheck : public MButton {
protected:
    StateType                State;
public:
			     MCheck (int x1, int y1, int x2, int y2, BYTE ALabel[], WORD AHandle, WORD AParent = HROOT);
			     MCheck (const RECT& ABox, BYTE ALabel[], WORD AHandle, WORD AParent = HROOT);
    virtual inline classType isA (void) {return (MCHECK_ID);};
    inline virtual void      SetState (StateType NewState);
    inline virtual StateType GetState (void);
    virtual void             Draw (void);
    virtual WORD             Handler (EVENT event);
			    ~MCheck (void);
};

#endif
