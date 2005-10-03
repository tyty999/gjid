#include "stdtest.h"

class CAppTest : public CApplication {
public:
    static CAppTest&	Instance (void);
};

/*static*/ CAppTest& CAppTest::Instance (void)
{
    static CAppTest s_App;
    return (s_App);
}

FbglMain (CAppTest)
