// tips.c

#include "funcs.h"

#include <shlwapi.h>


int Mes(LPCTSTR lpText)
{
    return MessageBox(NULL, lpText, NULL, MB_OK);
}

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
