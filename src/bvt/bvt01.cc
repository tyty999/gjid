#include "stdtest.h"
#include <time.h>

class CAppTest : public CApplication {
public:
    static CAppTest&	Instance (void);
protected:
    virtual void	OnIdle (void);
    virtual void	OnDraw (CGC& gc);
};

/*static*/ CAppTest& CAppTest::Instance (void)
{
    static CAppTest s_App;
    return (s_App);
}

void CAppTest::OnIdle (void)
{
    static size_t idleCount = 0;
    if (++idleCount > 25)
	Quit();
    cout << "OnIdle called" << endl;
}

void CAppTest::OnDraw (CGC& gc)
{
    CApplication::OnDraw (gc);
    foreach (CGC::iterator, i, gc)
	*i = distance(gc.begin(), i) % 128;
    cout << "OnDraw called" << endl;
}

FbglMain (CAppTest)
