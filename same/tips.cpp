// tips.cpp

#include "common.h"


// ==============================================
// 実装
// ==============================================

// 簡易メッセージボックス
int Mes( LPCTSTR lpText, LPCTSTR lpCaption, UINT uType, HWND hWnd )
{
	return MessageBox( hWnd, lpText, lpCaption, uType );
}

// カレントディレクトリの修正
bool SetCurDir( void )
{
	LPTSTR pstrDir;
	TCHAR strDir[MAX_PATH];

	if ( !GetModuleFileName( NULL, strDir, MAX_PATH ) )
		return false;

	pstrDir = StrRChr( strDir, NULL, '\\' );
	if ( !pstrDir )
		return false;

	lstrcpy( pstrDir, "" );

	if ( !SetCurrentDirectory( strDir ) )
		return false;

	return true;
}


// EOF