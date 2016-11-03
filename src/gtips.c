// gtips.c

#include "gtips.h"


Surface* createSurface(int const width, int const height) {
    HDC const hTemp = GetDC(NULL);

    Surface* const surface = malloc(sizeof(Surface));
    surface->hDC = CreateCompatibleDC(hTemp);
    surface->hBitmap = CreateCompatibleBitmap(hTemp, width, height);
    if (surface->hDC == NULL || surface->hBitmap == NULL) {
        destroySurface(surface);
        ReleaseDC(NULL, hTemp);
        return NULL;
    }
    SelectObject(surface->hDC, surface->hBitmap);

    ReleaseDC(NULL, hTemp);

    surface->width = width;
    surface->height = height;

    surfacePaintRect(surface, NULL, RGB(0, 0, 0));

    return surface;
}

void destroySurface(Surface* const surface) {
    if (surface == NULL) {
        return;
    }
    if (surface->hBitmap != NULL) {
        DeleteObject(surface->hBitmap);
    }
    if (surface->hDC != NULL) {
        DeleteDC(surface->hDC);
    }
    free(surface);
}

Surface* surfaceFromBitmapFile(char const* const fileName) {
    HDC const hTemp = GetDC(NULL);

    Surface* const surface = malloc(sizeof(Surface));
    surface->hDC = CreateCompatibleDC(hTemp);
    surface->hBitmap = (HBITMAP)LoadImage(NULL, fileName, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
    if (surface->hDC == NULL || surface->hBitmap == NULL) {
        destroySurface(surface);
        ReleaseDC(NULL, hTemp);
        return NULL;
    }
    SelectObject(surface->hDC, surface->hBitmap);

    ReleaseDC(NULL, hTemp);

    BITMAP bm;
    GetObject(surface->hBitmap, sizeof(BITMAP), &bm);
    surface->width = bm.bmWidth;
    surface->height = bm.bmHeight;

    return surface;
}

Surface* surfaceFromResource(HINSTANCE const hInst, char const* const resourceName) {
    HDC const hTemp = GetDC(NULL);

    Surface* const surface = malloc(sizeof(Surface));
    surface->hDC = CreateCompatibleDC(hTemp);
    surface->hBitmap = (HBITMAP)LoadImage(hInst, resourceName, IMAGE_BITMAP, 0, 0, LR_SHARED);
    if (surface->hDC == NULL || surface->hBitmap == NULL) {
        destroySurface(surface);
        ReleaseDC(NULL, hTemp);
        return NULL;
    }
    SelectObject(surface->hDC, surface->hBitmap);

    ReleaseDC(NULL, hTemp);

    BITMAP bm;
    GetObject(surface->hBitmap, sizeof(BITMAP), &bm);
    surface->width = bm.bmWidth;
    surface->height = bm.bmHeight;

    return surface;
}

void surfacePaintRect(Surface* const surface, LPRECT const rect, COLORREF const color) {
    HBRUSH const hBrush = CreateSolidBrush(color);

    HBRUSH const hOld = (HBRUSH)SelectObject(surface->hDC, hBrush);
    if (rect == NULL) {
        PatBlt(surface->hDC, 0, 0, surface->width, surface->height, PATCOPY);
    } else {
        PatBlt(surface->hDC, rect->left, rect->top, rect->right - rect->left,
               rect->bottom - rect->top, PATCOPY);
    }
    SelectObject(surface->hDC, hOld);

    DeleteObject(hBrush);
}

int surfaceDrawText(Surface* const surface, int const x, int const y, int const size,
        COLORREF const color, char const* text) {
    HFONT const hFont = CreateFont(size,
            0,
            0,
            0,
            600,
            0,
            0,
            0,
            SHIFTJIS_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_DONTCARE,
            "‚l‚r ƒSƒVƒbƒN");

    if (hFont == NULL) {
        return 0;
    }

    int const iOldMode = SetBkMode(surface->hDC, TRANSPARENT);
    HFONT const hOldFont = (HFONT)SelectObject(surface->hDC, hFont);
    COLORREF const clOld = SetTextColor(surface->hDC, color);

    TextOut(surface->hDC, x, y, text, lstrlen(text));

    SetTextColor(surface->hDC, clOld);
    SelectObject(surface->hDC, hOldFont);
    SetBkMode(surface->hDC, iOldMode);

    DeleteObject(hFont);

    return 1;
}

void surfaceBlit(Surface* const dest, int const dx, int const dy, int const width, int const height,
        Surface* const src, int const sx, int const sy, DWORD const mode) {
    BitBlt(dest->hDC, dx, dy, width, height, src->hDC, sx, sy, mode);
}
