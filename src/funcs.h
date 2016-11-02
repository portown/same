// funcs.h

#pragma once

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

// tips.c
int Mes(LPCTSTR);
int SetCurDir(void);

// gtips.c
void    InitSurface(HDC*, HBITMAP*, unsigned short, unsigned short);
void    RelsSurface(HDC*, HBITMAP*);
HBITMAP Load_Bmp(const char*);

void PaintRect(HDC, LPRECT, COLORREF);
int PutText(HDC, int, int, int, COLORREF, char const*);

#ifdef __cplusplus
}
#endif