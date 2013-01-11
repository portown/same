// funcs.h

#pragma once


// ==============================================
// ŽÀ‘•
// ==============================================

// mt19937ar.c
void init_genrand( unsigned long );
unsigned long genrand_int32( void );

// tips.cpp
int Mes( LPCTSTR = NULL, LPCTSTR = NULL, UINT = MB_OK, HWND = NULL );
bool SetCurDir( void );

// gtips.cpp
void InitSurface( HDC &, HBITMAP &, unsigned short, unsigned short );
void RelsSurface( HDC &, HBITMAP & );
HBITMAP Load_Bmp( const char * );
void PaintRect( HDC, LPRECT, COLORREF );
bool PutText( HDC, int, int, int, COLORREF, char *, int = 0 );


// EOF