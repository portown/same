// funcs.hxx

#ifndef FUNCS_HXX
#define FUNCS_HXX

#include <string_view>

#include <windows.h>


bool PutText(HDC, int, int, int, COLORREF, std::wstring_view str);

#endif  // FUNCS_HXX
