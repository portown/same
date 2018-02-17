// funcs.h

#pragma once

#include <windows.h>

#include "tstring_view.hxx"


bool PutText(HDC, int, int, int, COLORREF, tch::tstring_view str);
