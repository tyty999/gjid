/* graph_ds.h
**
**	Defines inline data structures for 'graph' routines.
*/

#ifndef __GRAPH_DS
#define __GRAPH_DS

class Point {
public:
    int			x;
    int			y;

public:
    inline		Point (void);
    inline		Point (int nx, int ny);
    inline void		Set (int nx, int ny);
    inline Point&	operator= (const Point& ToBe);
    inline Point&	operator+ (const Point& ToAdd);
};

class Point3d {
public:
    int			x;
    int			y;
    int			z;

public:
    inline		Point3d (void);
    inline		Point3d (int nx, int ny, int nz);
    inline void		Set (int nx, int ny, int nz);
    inline Point3d&	operator= (const Point3d& ToBe);
    inline Point3d&	operator+ (const Point3d& ToAdd);
};

typedef struct {
    int			x1;
    int			y1;
    int			x2;
    int			y2;	
} SimpleRect;
    
class Rectangle {
public:
    int			x1;
    int			y1;
    int			x2;
    int			y2;	
    int			width;
    int			height;

public:
    inline		Rectangle (void);
    inline		Rectangle (int left, int top, int right, int bottom);
    inline void		Set (int left, int top, int right, int bottom);
    inline Rectangle&	operator= (const Rectangle& ToBe);
    inline Rectangle&	operator= (const SimpleRect& ToBe);
};

///////////////////////////////////////////////////////////////////////////////

inline Point :: Point (void)
{
    x = y = 0;
}

inline Point :: Point (int nx, int ny)
{
    x = nx;
    y = ny;
}

inline void Point :: Set (int nx, int ny)
{
    x = nx;
    y = ny;
}

inline Point& Point :: operator= (const Point& ToBe)
{
    x = ToBe.x;
    y = ToBe.y;
    return (*this);
}

inline Point& Point :: operator+ (const Point& ToAdd)
{
    x += ToAdd.x;
    y += ToAdd.y;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////

inline Point3d :: Point3d (void)
{
    x = y = z = 0;
}

inline Point3d :: Point3d (int nx, int ny, int nz)
{
    x = nx;
    y = ny;
    z = nz;
}

inline void Point3d :: Set (int nx, int ny, int nz)
{
    x = nx;
    y = ny;
    z = nz;
}

inline Point3d& Point3d :: operator= (const Point3d& ToBe)
{
    x = ToBe.x;
    y = ToBe.y;
    z = ToBe.z;
    return (*this);
}

inline Point3d& Point3d :: operator+ (const Point3d& ToAdd)
{
    x += ToAdd.x;
    y += ToAdd.y;
    z += ToAdd.z;
    return (*this);
}

///////////////////////////////////////////////////////////////////////////////

inline Rectangle :: Rectangle (void)
{
    x1 = x2 = y1 = y2 = 0;
}

inline Rectangle :: Rectangle (int left, int top, int right, int bottom)
{
    x1 = left;
    x2 = right;
    y1 = top;
    y2 = bottom;
    width = x2 - x1;
    height = y2 - y1;
}

inline void Rectangle :: Set (int left, int top, int right, int bottom)
{
    x1 = left;
    x2 = right;
    y1 = top;
    y2 = bottom;
    width = x2 - x1;
    height = y2 - y1;
}

inline Rectangle& Rectangle :: operator= (const Rectangle& ToBe)
{
    x1 = ToBe.x1;
    x2 = ToBe.x2;
    y1 = ToBe.y1;
    y2 = ToBe.y2;
    width = ToBe.width;
    height = ToBe.height;
    return (*this);
}

inline Rectangle& Rectangle :: operator= (const SimpleRect& ToBe)
{
    x1 = ToBe.x1;
    x2 = ToBe.x2;
    y1 = ToBe.y1;
    y2 = ToBe.y2;
    width = x2 - x1;
    height = y2 - y1;
    return (*this);
}

#endif

