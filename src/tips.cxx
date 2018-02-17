// tips.cpp

#include "common.hxx"


int Mes(LPCTSTR lpText, LPCTSTR lpCaption, UINT uType, HWND hWnd)
{
    return MessageBox(hWnd, lpText, lpCaption, uType);
}

bool SetCurDir(void)
{
    LPTSTR pstrDir;
    TCHAR  strDir[MAX_PATH];

    if (!GetModuleFileName(nullptr, strDir, MAX_PATH))
        return false;

    pstrDir = StrRChr(strDir, nullptr, '\\');
    if (!pstrDir)
        return false;

    lstrcpy(pstrDir, TEXT(""));

    if (!SetCurrentDirectory(strDir))
        return false;

    return true;
}
