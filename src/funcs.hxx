// funcs.h

#pragma once


// ==============================================
// ŽÀ‘•
// ==============================================

// tips.cpp
extern "C" {
    int Mes(LPCTSTR = NULL, LPCTSTR = NULL, UINT = MB_OK, HWND = NULL);
    int SetCurDir(void);
}

// gtips.cpp
void    InitSurface(HDC&, HBITMAP&, unsigned short, unsigned short);
void    RelsSurface(HDC&, HBITMAP&);
HBITMAP Load_Bmp(const char*);

void PaintRect(HDC, LPRECT, COLORREF);
bool PutText(HDC, int, int, int, COLORREF, char const*, int = 0);


// EOF
