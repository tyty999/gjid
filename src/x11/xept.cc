// This file is part of the fbgl library.
//
// Copyright (C) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.
// 
// xept.cc
//

#include "xept.h"
using namespace ustl;

//----------------------------------------------------------------------

static const char* RequestCodeText (unsigned int code);

//----------------------------------------------------------------------

XlibError::XlibError (void) throw()
: runtime_error (""),
  m_Event ()
{
    memset (&m_Event, 0, sizeof(m_Event));
}

XlibError::XlibError (const XErrorEvent& e) throw()
: runtime_error (""),
  m_Event (e)
{
    try { m_Arg.resize (512); } catch (...) {}
    if (m_Arg.begin() && m_Arg.size() > 16) {
	fill (m_Arg, '\x0');
	XGetErrorText (m_Event.display, m_Event.error_code, m_Arg.begin(), m_Arg.writable_size());
    }
}

const char* XlibError::what (void) const throw()
{
    return ("Xlib error");
}

void XlibError::info (ustl::string& msgbuf, const char* fmt) const throw()
{
    if (!fmt) fmt = "%s: %s";
    const char* errorText = m_Arg.begin() ? m_Arg.begin() : "[out of memory]";
    try { msgbuf.format (fmt, RequestCodeText(m_Event.request_code), errorText); } catch (...) {}
}

void XlibError::read (ustl::istream& is)
{
    runtime_error::read (is);
    is.read (&m_Event, sizeof(m_Event));
    is.align();
}

void XlibError::write (ustl::ostream& os) const
{
    runtime_error::write (os);
    os.write (&m_Event, sizeof(m_Event));
    os.align();
}

size_t XlibError::stream_size (void) const
{
    return (runtime_error::stream_size() + Align (sizeof (m_Event)));
}

//----------------------------------------------------------------------

static const char* RequestCodeText (unsigned int code)
{
    static const char* rct[] = {
	"X_NullOperation",
	"X_CreateWindow",
	"X_ChangeWindowAttributes",
	"X_GetWindowAttributes",
	"X_DestroyWindow",
	"X_DestroySubwindows",
	"X_ChangeSaveSet",
	"X_ReparentWindow",
	"X_MapWindow",
	"X_MapSubwindows",
	"X_UnmapWindow",
	"X_UnmapSubwindows",
	"X_ConfigureWindow",
	"X_CirculateWindow",
	"X_GetGeometry",
	"X_QueryTree",
	"X_InternAtom",
	"X_GetAtomName",
	"X_ChangeProperty",
	"X_DeleteProperty",
	"X_GetProperty",
	"X_ListProperties",
	"X_SetSelectionOwner",
	"X_GetSelectionOwner",
	"X_ConvertSelection",
	"X_SendEvent",
	"X_GrabPointer",
	"X_UngrabPointer",
	"X_GrabButton",
	"X_UngrabButton",
	"X_ChangeActivePointerGrab",
	"X_GrabKeyboard",
	"X_UngrabKeyboard",
	"X_GrabKey",
	"X_UngrabKey",
	"X_AllowEvents",
	"X_GrabServer",
	"X_UngrabServer",
	"X_QueryPointer",
	"X_GetMotionEvents",
	"X_TranslateCoords",
	"X_WarpPointer",
	"X_SetInputFocus",
	"X_GetInputFocus",
	"X_QueryKeymap",
	"X_OpenFont",
	"X_CloseFont",
	"X_QueryFont",
	"X_QueryTextExtents",
	"X_ListFonts",
	"X_ListFontsWithInfo",
	"X_SetFontPath",
	"X_GetFontPath",
	"X_CreatePixmap",
	"X_FreePixmap",
	"X_CreateGC",
	"X_ChangeGC",
	"X_CopyGC",
	"X_SetDashes",
	"X_SetClipRectangles",
	"X_FreeGC",
	"X_ClearArea",
	"X_CopyArea",
	"X_CopyPlane",
	"X_PolyPoint",
	"X_PolyLine",
	"X_PolySegment",
	"X_PolyRectangle",
	"X_PolyArc",
	"X_FillPoly",
	"X_PolyFillRectangle",
	"X_PolyFillArc",
	"X_PutImage",
	"X_GetImage",
	"X_PolyText8",
	"X_PolyText16",
	"X_ImageText8",
	"X_ImageText16",
	"X_CreateColormap",
	"X_FreeColormap",
	"X_CopyColormapAndFree",
	"X_InstallColormap",
	"X_UninstallColormap",
	"X_ListInstalledColormaps",
	"X_AllocColor",
	"X_AllocNamedColor",
	"X_AllocColorCells",
	"X_AllocColorPlanes",
	"X_FreeColors",
	"X_StoreColors",
	"X_StoreNamedColor",
	"X_QueryColors",
	"X_LookupColor",
	"X_CreateCursor",
	"X_CreateGlyphCursor",
	"X_FreeCursor",
	"X_RecolorCursor",
	"X_QueryBestSize",
	"X_QueryExtension",
	"X_ListExtensions",
	"X_ChangeKeyboardMapping",
	"X_GetKeyboardMapping",
	"X_ChangeKeyboardControl",
	"X_GetKeyboardControl",
	"X_Bell",
	"X_ChangePointerControl",
	"X_GetPointerControl",
	"X_SetScreenSaver",
	"X_GetScreenSaver",
	"X_ChangeHosts",
	"X_ListHosts",
	"X_SetAccessControl",
	"X_SetCloseDownMode",
	"X_KillClient",
	"X_RotateProperties",
	"X_ForceScreenSaver",
	"X_SetPointerMapping",
	"X_GetPointerMapping",
	"X_SetModifierMapping",
	"X_GetModifierMapping",
	"X_NoOperation"
    };
    return (rct [min (code, VectorSize(rct)-1)]);
}

