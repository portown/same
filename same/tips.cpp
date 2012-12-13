// tips.cpp

#include "common.h"


// ==============================================
// ����
// ==============================================

// �ȈՃ��b�Z�[�W�{�b�N�X
int Mes( LPCTSTR lpText, LPCTSTR lpCaption, UINT uType, HWND hWnd )
{
	return MessageBox( hWnd, lpText, lpCaption, uType );
}

// �J�����g�f�B���N�g���̏C��
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