
#ifndef __XCAMERA_H
#define __XCAMERA_H

#include <xcanvas.h>
#include <vector.h>		     
#include <3d.h>

typedef enum {
    Parallel,
    Perspective,
} ProjectionType;

class XCamera : public XCanvas {
private:
    int			FocalLength;
    Point		Position;
    Vector<int>		ViewAngle;		// Where the camera points

protected:
    ProjectionType	Projection;
     
private:
    virtual void	TransformPoint (Point * pt);
    
public:
			XCamera (int x, int y, int width, int height);
    virtual void	MoveTo (int x, int y, int z);
    virtual void	PointTo (int anglex, int angley, int anglez);
    BOOL		CanSeeFace (const Vector<int>& FaceNormal);
    
    void		DrawPixel3d (int x, int y, int z);
    void		DrawLine3d (int x1, int y1, int z1, int x2, int y2, int z2);
    void		DrawPolygon3d (const Point * points, int nPts, int type = Complex);
    void		FillPolygon3d (const Point * points, int nPts, int type = Complex);
};

#endif

