// funcs.h

#pragma once

#include "same/ui/surface.hxx"


// ==============================================
// ŽÀ‘•
// ==============================================

// tips.cpp
int Mes(LPCTSTR = nullptr, LPCTSTR = nullptr, UINT = MB_OK, HWND = nullptr);
bool SetCurDir(void);

// gtips.cpp
bool PutText(HDC, int, int, int, COLORREF, char const*, int = 0);


// EOF
