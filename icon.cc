// This file is part of the GJID game.
//
// Copyright (C) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.
// 
// icon.cc
//
// 	Implements the Icon class
//

#include "icon.h"

/// Default constructor.
Icon::Icon (dim_t w, dim_t h, const color_t* p)
: CImage ()
{
    SetImage (w, h, p);
}

/// Copies data from the given source.
void Icon::SetImage (dim_t w, dim_t h, const color_t* p)
{
    Resize (Size2d (w, h));
    if (p)
	copy_n (p, w * h, begin());
}

