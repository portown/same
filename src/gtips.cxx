// gtips.cpp

// インクルード
#include "common.hxx"

// 関数のプロトタイプ宣言
HPALETTE SetPalette(LPBITMAPINFOHEADER);
int      MakeBmpInfo(LPBITMAPINFOHEADER, LPBITMAPINFO, char*);


// ==============================================
// 画像処理関数
// ==============================================

// デバイスコンテキスト・ビットマップハンドルの初期化
void InitSurface(HDC& hDC, HBITMAP& hBm, unsigned short w, unsigned short h)
{
    BITMAPINFO bi;
    RECT       rc;
    HDC        hTemp;

    hTemp = GetDC(NULL);

    bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth       = w;
    bi.bmiHeader.biHeight      = -h;
    bi.bmiHeader.biPlanes      = 1;
    bi.bmiHeader.biBitCount    = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    hDC = CreateCompatibleDC(hTemp);
    hBm = CreateDIBSection(hTemp, &bi, DIB_RGB_COLORS, NULL, NULL, 0);
    if (hBm == NULL)
    {
        Mes("サーフェイスを正しく作成できませんでした");
        return;
    }
    SelectObject(hDC, hBm);

    ReleaseDC(NULL, hTemp);

    rc.left   = rc.top = 0;
    rc.right  = w;
    rc.bottom = h;
    PaintRect(hDC, &rc, RGB(0, 0, 0));
}

// デバイスコンテキスト・ビットマップハンドルの解放
void RelsSurface(HDC& hDC, HBITMAP& hBm)
{
    if (hBm)
    {
        DeleteObject(hBm);
        hBm = NULL;
    }
    if (hDC)
    {
        DeleteDC(hDC);
        hDC = NULL;
    }
}

// 矩形塗りつぶし
void PaintRect(HDC hDC, LPRECT lprc, COLORREF col)
{
    HBRUSH hBrush, hOld;

    hBrush = CreateSolidBrush(col);

    hOld = ( HBRUSH )SelectObject(hDC, hBrush);
    if (lprc == NULL)
    {
        lprc = new RECT;
        SetRect(lprc, 0, 0, 640, 480);
        PatBlt(hDC, lprc->left, lprc->top, lprc->right - lprc->left,
               lprc->bottom - lprc->top, PATCOPY);
        delete lprc;
    }
    else
    {
        PatBlt(hDC, lprc->left, lprc->top, lprc->right - lprc->left,
               lprc->bottom - lprc->top, PATCOPY);
    }
    SelectObject(hDC, hOld);

    DeleteObject(hBrush);
}

// 文字の描画
bool PutText(HDC hDC, int x, int y, int f_size,
             COLORREF col, char* str, int iLen)
{
    COLORREF clOld;
    HFONT    hFont, hOldFont;
    int      iOldMode;

    if (iLen == 0) iLen = lstrlen(str);

    hFont = CreateFont(f_size,
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
                       "ＭＳ ゴシック");

    if (!hFont)
        return false;

    iOldMode = SetBkMode(hDC, TRANSPARENT);
    hOldFont = ( HFONT )SelectObject(hDC, hFont);
    clOld    = SetTextColor(hDC, col);

    TextOut(hDC, x, y, str, iLen);

    SetTextColor(hDC, clOld);
    SelectObject(hDC, hOldFont);
    SetBkMode(hDC, iOldMode);

    DeleteObject(hFont);

    return true;
}

// 画像の読み込み
HBITMAP Load_Bmp(const char* f_name)
{
    LPBITMAPFILEHEADER lpBf;
    LPBITMAPINFOHEADER lpBi;
    LPBITMAPINFO       lpBmpInfo;
    HPALETTE           hPalette;
    HBITMAP            hBm;
    HANDLE             hFile;
    HANDLE             hMemFile, hMemIfHd, hMemInfo, hMemBuff;
    DWORD              dwRead;
    BOOL               bRet;
    char*              lpBuffer;
    //char szErr[0x100];
    char szType[3];
    int  i;

    return ( HBITMAP )LoadImage(NULL, f_name, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);

    hFile = CreateFile(f_name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        /*wsprintf( szErr, "【%s】：ビットマップファイルの読み込みに失敗しました", f_name );
           tips::Mes( szErr );*/
        return NULL;
    }

    hMemFile = GlobalAlloc(GHND, sizeof(BITMAPFILEHEADER));
    lpBf     = ( LPBITMAPFILEHEADER )GlobalLock(hMemFile);

    SetFilePointer(hFile, 0, 0, FILE_BEGIN);
    bRet = ReadFile(hFile, ( LPVOID )lpBf, sizeof(BITMAPFILEHEADER), &dwRead, NULL);
    if (!bRet)
    {
        GlobalUnlock(hMemFile);
        GlobalFree(hMemFile);
        CloseHandle(hFile);
        return NULL;
    }

    szType[0] = LOBYTE(lpBf->bfType);
    szType[1] = HIBYTE(lpBf->bfType);
    szType[2] = 0x00;
    if (lstrcmp(szType, "BM"))
    {
        //tips::Mes( "ビットマップファイルではありません" );
        GlobalUnlock(hMemFile);
        GlobalFree(hMemFile);
        CloseHandle(hFile);
        return NULL;
    }

    hMemIfHd = GlobalAlloc(GHND, sizeof(BITMAPINFOHEADER));
    lpBi     = ( LPBITMAPINFOHEADER )GlobalLock(hMemIfHd);

    bRet = ReadFile(hFile, ( LPVOID )lpBi, sizeof(BITMAPINFOHEADER), &dwRead, NULL);
    if (!bRet)
    {
        GlobalUnlock(hMemIfHd);
        GlobalFree(hMemIfHd);
        GlobalUnlock(hMemFile);
        GlobalFree(hMemFile);
        CloseHandle(hFile);
        return NULL;
    }

    if (lpBi->biBitCount < 16 && lpBi->biClrUsed == 0)
    {
        lpBi->biClrUsed = 1;

        for (i = 0; i < lpBi->biBitCount; i++)
        {
            lpBi->biClrUsed *= 2;
        }
    }

    hMemInfo = GlobalAlloc(GHND,
                           sizeof(BITMAPINFO) + lpBi->biClrUsed * sizeof(RGBQUAD));
    lpBmpInfo = ( LPBITMAPINFO )GlobalLock(hMemInfo);

    hMemBuff = GlobalAlloc(GHND, lpBf->bfSize - sizeof(BITMAPFILEHEADER));
    lpBuffer = ( char* )GlobalLock(hMemBuff);

    SetFilePointer(hFile, sizeof(BITMAPFILEHEADER), 0, FILE_BEGIN);
    bRet = ReadFile(hFile, ( LPVOID )lpBuffer,
                    lpBf->bfSize - lpBf->bfOffBits + lpBi->biClrUsed
                    * sizeof(RGBQUAD) + sizeof(BITMAPINFOHEADER),
                    &dwRead, NULL);
    if (!bRet)
    {
        GlobalUnlock(hMemBuff);
        GlobalFree(hMemBuff);
        GlobalUnlock(hMemInfo);
        GlobalFree(hMemInfo);
        GlobalUnlock(hMemIfHd);
        GlobalFree(hMemIfHd);
        GlobalUnlock(hMemFile);
        GlobalFree(hMemFile);
        CloseHandle(hFile);
        return NULL;
    }

    if (lpBi->biClrUsed != 0 || lpBi->biBitCount != 24)
        hPalette = SetPalette(lpBi);

    lpBmpInfo->bmiHeader = *lpBi;
    MakeBmpInfo(lpBi, lpBmpInfo, lpBuffer);

    GlobalUnlock(hMemBuff);
    GlobalFree(hMemBuff);
    lpBuffer = NULL;

    hBm = CreateDIBSection(NULL, lpBmpInfo, DIB_RGB_COLORS, ( void** )&lpBuffer, NULL, 0);
    if (hBm == NULL)
    {
        GlobalUnlock(hMemInfo);
        GlobalFree(hMemInfo);
        GlobalUnlock(hMemIfHd);
        GlobalFree(hMemIfHd);
        GlobalUnlock(hMemFile);
        GlobalFree(hMemFile);
        CloseHandle(hFile);
        DeleteObject(hPalette);
        return NULL;
    }

    SetFilePointer(hFile, lpBf->bfOffBits, 0, FILE_BEGIN);
    bRet = ReadFile(hFile, ( LPVOID )lpBuffer,
                    lpBf->bfSize - lpBf->bfOffBits /* - lpBi->biClrUsed * sizeof( RGBQUAD )*/,
                    &dwRead, NULL);
    if (!bRet)
    {
        GlobalUnlock(hMemInfo);
        GlobalFree(hMemInfo);
        GlobalUnlock(hMemIfHd);
        GlobalFree(hMemIfHd);
        GlobalUnlock(hMemFile);
        GlobalFree(hMemFile);
        CloseHandle(hFile);
        DeleteObject(hPalette);
        return NULL;
    }

    GlobalUnlock(hMemInfo);
    GlobalFree(hMemInfo);

    GlobalUnlock(hMemIfHd);
    GlobalFree(hMemIfHd);

    GlobalUnlock(hMemFile);
    GlobalFree(hMemFile);

    DeleteObject(hPalette);

    CloseHandle(hFile);

    return hBm;
}

// パレットの設定
HPALETTE SetPalette(LPBITMAPINFOHEADER lpBi)
{
    LPLOGPALETTE lpPal;
    LPRGBQUAD    lpRGB;
    HANDLE       hPal;
    HPALETTE     hPalette;
    WORD         i;

    hPal = GlobalAlloc(GHND, sizeof(LOGPALETTE)
                       + lpBi->biClrUsed * sizeof(PALETTEENTRY));
    lpPal = ( LPLOGPALETTE )GlobalLock(hPal);

    lpPal->palVersion    = 0x300;
    lpPal->palNumEntries = ( WORD )lpBi->biClrUsed;

    lpRGB = ( LPRGBQUAD )(( LPSTR )lpBi + lpBi->biSize);

    for (i = 0; i < lpBi->biClrUsed; i++, lpRGB++)
    {
        lpPal->palPalEntry[i].peRed   = lpRGB->rgbRed;
        lpPal->palPalEntry[i].peGreen = lpRGB->rgbGreen;
        lpPal->palPalEntry[i].peBlue  = lpRGB->rgbBlue;
        lpPal->palPalEntry[i].peFlags = 0;
    }

    GlobalUnlock(hPal);
    hPalette = CreatePalette(lpPal);
    /*if ( !hPalette )
       {
       tips::Mes( "パレットの作成に失敗しました" );
       }*/

    GlobalFree(hPal);

    return hPalette;
}

// BITMAPINFOのデータ入力
int MakeBmpInfo(LPBITMAPINFOHEADER lpBi, LPBITMAPINFO lpBmpInfo, char* lpBuffer)
{
    LPRGBQUAD lpRGB;
    int       i;

    lpRGB = ( LPRGBQUAD )(lpBuffer + sizeof(BITMAPINFOHEADER));

    for (i = 0; i < ( int )lpBi->biClrUsed; i++)
    {
        lpBmpInfo->bmiColors[i].rgbRed      = lpRGB->rgbRed;
        lpBmpInfo->bmiColors[i].rgbGreen    = lpRGB->rgbGreen;
        lpBmpInfo->bmiColors[i].rgbBlue     = lpRGB->rgbBlue;
        lpBmpInfo->bmiColors[i].rgbReserved = 0;
        lpRGB++;
    }

    return 1;
}

// EOF
