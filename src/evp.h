// evp.h
//

#ifndef EVP_H_3F79AF7B241A85637686DA127727B280
#define EVP_H_3F79AF7B241A85637686DA127727B280

#include "primi.h"

namespace fbgl {

/// Special key codes given to OnKey
enum EKeyDataValue {
    kv_Space = ' ',
    kv_Tab = '\t',
    kv_Enter = '\n',
    kv_First = 0xE000,		// 0xE000 - 0xF8FF private use unicode region
    kv_Esc = kv_First,
    kv_Backspace,
    kv_Center,
    kv_Close,
    kv_Delete,
    kv_Down,
    kv_DownLeft,
    kv_DownRight,
    kv_End,
    kv_F0,
    kv_F1,
    kv_F2,
    kv_F3,
    kv_F4,
    kv_F5,
    kv_F6,
    kv_F7,
    kv_F8,
    kv_F9,
    kv_F10,
    kv_F11,
    kv_F12,
    kv_Home,
    kv_Insert,
    kv_Left,
    kv_PageDown,
    kv_PageUp,
    kv_Right,
    kv_Up,
    kv_UpLeft,
    kv_UpRight,
    kv_Last,
    kv_nKeys = (kv_Last - kv_First)
};

/// Key state flags.
enum EKeyState {
    ks_Alt,
    ks_Ctrl,
    ks_Shift,
    ks_Last
};

typedef wchar_t		key_t;		///< Used for keycodes.
typedef bitset<ks_Last>	keystate_t;	///< See EKeyState for bits.
typedef uint32_t	bidx_t;		///< Mouse button index.

/// \class CEventProcessor evp.h fbgl.h
///
/// \brief Event processor interface.
///
class CEventProcessor {
public:
    inline virtual     ~CEventProcessor (void) {}
    inline virtual void	OnKey (key_t, keystate_t) {}
    inline virtual void	OnMouseMove (coord_t, coord_t, keystate_t) {}
    inline virtual void	OnButtonDown (bidx_t, coord_t, coord_t, keystate_t) {}
};

} // namespace fbgl

CAST_STREAMABLE (fbgl::EKeyDataValue, fbgl::key_t)

#endif

