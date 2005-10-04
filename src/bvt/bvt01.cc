#include "stdtest.h"

class CAppTest : public CApplication {
public:
    static CAppTest&	Instance (void);
protected:
    virtual void	OnIdle (void);
};

/*static*/ CAppTest& CAppTest::Instance (void)
{
    static CAppTest s_App;
    return (s_App);
}

void CAppTest::OnIdle (void)
{
    Quit();
}

FbglMain (CAppTest)
