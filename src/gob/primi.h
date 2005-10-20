// This file is part of the fbgl library.
//
// Copyright (C) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.
// 
// primi.h
//
//	Geometric primitive types.
//

#ifndef PRIMI_H_71C24D056D1C3A4C09C315734ABA986D
#define PRIMI_H_71C24D056D1C3A4C09C315734ABA986D

#include "pal.h"

namespace fbgl {

//----------------------------------------------------------------------

typedef int16_t			coord_t;	///< Scaled coordinate
typedef uint16_t		dim_t;		///< Linear dimension, scaled

typedef tuple<2, coord_t>	Point;
typedef tuple<2, dim_t>		Size2d;

//----------------------------------------------------------------------

/// \class Rect primi.h fbgl.h
/// \brief Defines a rectangle in scaled coordinates.
class Rect : public tuple<2, Point> {
public:
			Rect (const Rect& r)
			    : tuple<2, Point> (r) {}
    explicit		Rect (coord_t sx1 = 0, coord_t sy1 = 0, coord_t sx2 = 0, coord_t sy2 = 0)
			    : tuple<2, Point> (Point (sx1, sy1), Point (sx2, sy2)) {}
			Rect (const Point& stl, const Point& sbr)
			    : tuple<2, Point> (stl, sbr) { }
			Rect (const Point& stl, const Size2d& swh)
			    : tuple<2, Point> (stl, stl) { at(1) += swh; }
    inline dim_t	Width (void) const	{ return (at(1)[0] - at(0)[0]); }
    inline dim_t	Height (void) const	{ return (at(1)[1] - at(0)[1]); }
    inline Size2d	Size (void) const	{ return (at(1) - at(0)); }
    inline bool		Empty (void) const	{ return (!Height() || !Width()); }
};

//----------------------------------------------------------------------

} // namespace fbgl

TUPLE_NUMERIC_LIMITS (fbgl::Point)
TUPLE_NUMERIC_LIMITS (fbgl::Size2d)
TUPLE_NUMERIC_LIMITS (fbgl::Rect)

#endif

