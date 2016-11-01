// tips.c

#include <windows.h>
#include <shlwapi.h>


// ==============================================
// 実装
// ==============================================

// 簡易メッセージボックス
int Mes(LPCTSTR lpText, LPCTSTR lpCaption, UINT uType, HWND hWnd)
{
    return MessageBox(hWnd, lpText, lpCaption, uType);
}

// カレントディレクトリの修正
int SetCurDir(void)
{
    LPTSTR pstrDir;
    TCHAR  strDir[MAX_PATH];

    if (!GetModuleFileName(NULL, strDir, MAX_PATH))
        return 0;

    pstrDir = StrRChr(strDir, NULL, '\\');
    if (!pstrDir)
        return 0;

    lstrcpy(pstrDir, "");

    if (!SetCurrentDirectory(strDir))
        return 0;

    return 1;
}

// EOF
