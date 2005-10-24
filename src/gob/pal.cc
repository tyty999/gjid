// This file is part of the fbgl library.
//
// Copyright (C) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.
// 
// pal.cc

#include "pal.h"

namespace fbgl {

color_t CPalette::AllocColor (colordef_t c)
{
    for (uoff_t i = 0; i < size(); ++ i)
	if (at(i) == c)
	    return (i);
    assert (size() < numeric_limits<color_t>::max() && "You are allocating too many colors; some of them would use other colors instead.");
    if (size() < numeric_limits<color_t>::max())
	push_back (c);
    return (size() - 1);
}

} // namespace fbgl

