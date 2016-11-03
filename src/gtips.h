// gtips.h

#pragma once

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct SurfaceTag {
    HDC hDC;
    HBITMAP hBitmap;
    int width;
    int height;
} Surface;

Surface* createSurface(int width, int height);
void destroySurface(Surface* surface);

Surface* surfaceFromBitmapFile(char const* fileName);
Surface* surfaceFromResource(HINSTANCE hInst, char const* resourceName);

void surfacePaintRect(Surface* surface, RECT* rect, COLORREF color);
int surfaceDrawText(Surface* surface, int left, int top, int size, COLORREF color, char const* text);
void surfaceBlit(Surface* dest, int dx, int dy, int width, int height,
        Surface* src, int sx, int sy, DWORD mode);


#ifdef __cplusplus
}
#endif
