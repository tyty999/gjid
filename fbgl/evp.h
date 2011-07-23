// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "primi.h"

/// \class CEventProcessor evp.h fbgl.h
///
/// \brief Event processor interface.
///
class CEventProcessor {
public:
    /// Special key codes given to OnKey
    enum EKeyDataValue {
	key_Space = ' ',
	key_Tab = '\t',
	key_Enter = '\n',
	key_First = 0xE000,		// 0xE000 - 0xF8FF private use unicode region
	key_Esc = key_First,
	key_Backspace,
	key_Center,
	key_Close,
	key_Delete,
	key_Down,
	key_DownLeft,
	key_DownRight,
	key_End,
	key_F0,
	key_F1,
	key_F2,
	key_F3,
	key_F4,
	key_F5,
	key_F6,
	key_F7,
	key_F8,
	key_F9,
	key_F10,
	key_F11,
	key_F12,
	key_Home,
	key_Insert,
	key_Left,
	key_PageDown,
	key_PageUp,
	key_Right,
	key_Up,
	key_UpLeft,
	key_UpRight,
	key_Last,
	key_nKeys = (key_Last - key_First)
    };
    /// Key state flags.
    enum EKeyState {
	ks_Alt,
	ks_Ctrl,
	ks_Shift,
	ks_Last
    };
    typedef wchar_t		key_t;		///< Used for keycodes.
    typedef uint32_t		bidx_t;		///< Mouse button index.
public:
    inline virtual	~CEventProcessor (void) {}
    inline virtual void	OnKey (key_t) {}
    inline virtual void	OnMouseMove (coord_t, coord_t) {}
    inline virtual void	OnButtonDown (bidx_t, coord_t, coord_t) {}
};

CAST_STREAMABLE (CEventProcessor::EKeyDataValue, CEventProcessor::key_t)
