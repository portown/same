// funcs.h

#pragma once

#include "same/ui/surface.hxx"
#include "tstring_view.hxx"


// tips.cpp
int Mes(LPCTSTR = nullptr, LPCTSTR = nullptr, UINT = MB_OK, HWND = nullptr);
bool SetCurDir(void);

// gtips.cpp
bool PutText(HDC, int, int, int, COLORREF, tch::tstring_view str);
