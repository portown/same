// surface.h

#pragma once

#include <windows.h>


typedef struct SurfaceTag {
    HDC hDC;
    HBITMAP hBitmap;
} Surface;
