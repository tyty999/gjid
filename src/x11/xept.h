// xept.h
//

#ifndef XEPT_H_70C75AE21AC13FBB0069942B3BCA8A76
#define XEPT_H_70C75AE21AC13FBB0069942B3BCA8A76

#include <ustl.h>
#include <X11/Xlib.h>

class XlibError : public ustl::runtime_error {
public:
			XlibError (void) throw();
			XlibError (const XErrorEvent& e) throw();
    inline virtual     ~XlibError (void) throw() {}
    virtual const char*	what (void) const throw();
    virtual void	info (ustl::string& msgbuf, const char* fmt = NULL) const throw();
    virtual void	read (ustl::istream& is);
    virtual void	write (ustl::ostream& os) const;
    virtual size_t	stream_size (void) const;
public:
    XErrorEvent		m_Event;
};

#endif

