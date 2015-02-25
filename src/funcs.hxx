// funcs.h

#pragma once

#include "same/ui/surface.hxx"


// ==============================================
// ŽÀ‘•
// ==============================================

// mt19937ar.c
void          init_genrand(unsigned long);
unsigned long genrand_int32(void);

// tips.cpp
int Mes(LPCTSTR = NULL, LPCTSTR = NULL, UINT = MB_OK, HWND = NULL);
bool SetCurDir(void);

// gtips.cpp
std::shared_ptr<same::ui::Surface> InitSurface(unsigned short, unsigned short);

void PaintRect(HDC, LPRECT, COLORREF);
bool PutText(HDC, int, int, int, COLORREF, char const*, int = 0);


// EOF
