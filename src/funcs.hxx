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
bool PutText(HDC, int, int, int, COLORREF, char const*, int = 0);


// EOF
