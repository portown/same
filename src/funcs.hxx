// funcs.hxx

#ifndef FUNCS_HXX
#define FUNCS_HXX

#include <windows.h>

#include "tstring_view.hxx"


bool PutText(HDC, int, int, int, COLORREF, tch::tstring_view str);

#endif  // FUNCS_HXX
