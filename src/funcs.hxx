// funcs.h

#pragma once


// ==============================================
// ŽÀ‘•
// ==============================================

// tips.cpp
int Mes(LPCTSTR = NULL, LPCTSTR = NULL, UINT = MB_OK, HWND = NULL);
bool SetCurDir(void);

// gtips.cpp
void    InitSurface(HDC&, HBITMAP&, unsigned short, unsigned short);
void    RelsSurface(HDC&, HBITMAP&);
HBITMAP Load_Bmp(const char*);

void PaintRect(HDC, LPRECT, COLORREF);
bool PutText(HDC, int, int, int, COLORREF, char const*, int = 0);


// EOF
