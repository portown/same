// menu.cpp

#include "common.hxx"


// ==============================================
// 実装
// ==============================================

// コンストラクタ
CMENU::CMENU( unsigned short width, unsigned short height )
{
	m_Level = 0;

	LoadStatus();

	m_Width = width;
	m_Height = height;
	m_Sel = 0;
	m_MaskNum = 0;
	m_RepNum = -1;

	SetRect( &m_rcMenu, 230, 260, 230 + 180, 260 + 160 );
	SetRect( &m_rcLeft, m_rcMenu.right, m_rcMenu.top, m_rcMenu.right + 20, m_rcMenu.bottom );
	SetRect( &m_rcRight, m_rcLeft.left + 80, m_rcLeft.top, m_rcLeft.right + 80, m_rcLeft.bottom );

	m_hDC = CreateCompatibleDC( NULL );
	m_hBm = Load_Bmp( DATA( title.bmp ) );
	SelectObject( m_hDC, m_hBm );

	m_hMenuDC = CreateCompatibleDC( NULL );
	m_hMenuBm = Load_Bmp( DATA( menu.bmp ) );
	SelectObject( m_hMenuDC, m_hMenuBm );
}

// 描画
void CMENU::Draw( HDC hDC )
{
	int i;

	BitBlt( hDC, 0, 0, m_Width, m_Height, m_hDC, 0, 0, SRCCOPY );

	for ( i = 0; i < 4; ++i )
	{
		if ( i == 1 && m_Level < 1 ) continue;

		if ( i + 1 == m_Sel )
		{
			BitBlt( hDC, m_rcMenu.left, m_rcMenu.top + i * ( m_rcMenu.bottom - m_rcMenu.top ) / 4,
				m_rcMenu.right - m_rcMenu.left, ( m_rcMenu.bottom - m_rcMenu.top ) / 4,
				m_hMenuDC, m_rcMenu.right - m_rcMenu.left, i * ( m_rcMenu.bottom - m_rcMenu.top ) / 4, SRCCOPY );
		}
		else
		{
			BitBlt( hDC, m_rcMenu.left, m_rcMenu.top + i * ( m_rcMenu.bottom - m_rcMenu.top ) / 4,
				m_rcMenu.right - m_rcMenu.left, ( m_rcMenu.bottom - m_rcMenu.top ) / 4,
				m_hMenuDC, 0, i * ( m_rcMenu.bottom - m_rcMenu.top ) / 4, SRCCOPY );
		}
	}

	if ( m_Level >= 1 )
	{
		if ( m_Sel == 6 )
		{
			BitBlt( hDC, m_rcLeft.left, m_rcLeft.top + ( m_rcLeft.bottom - m_rcLeft.top ) / 4,
				20, 40, m_hMenuDC, 40, 160, SRCCOPY );
		}
		else
		{
			BitBlt( hDC, m_rcLeft.left, m_rcLeft.top + ( m_rcLeft.bottom - m_rcLeft.top ) / 4,
				20, 40, m_hMenuDC, 0, 160, SRCCOPY );
		}
		if ( m_Sel == 10 )
		{
			BitBlt( hDC, m_rcRight.left, m_rcRight.top + ( m_rcRight.bottom - m_rcRight.top ) / 4,
				20, 40, m_hMenuDC, 60, 160, SRCCOPY );
		}
		else
		{
			BitBlt( hDC, m_rcRight.left, m_rcRight.top + ( m_rcRight.bottom - m_rcRight.top ) / 4,
				20, 40, m_hMenuDC, 20, 160, SRCCOPY );
		}

		BitBlt( hDC, m_rcLeft.right + 36, m_rcLeft.top + ( m_rcLeft.bottom - m_rcLeft.top ) / 4,
			19, 40, m_hMenuDC, 80 + m_MaskNum * 19, 160, SRCCOPY );
	}

	if ( m_Sel == 7 )
	{
		BitBlt( hDC, m_rcLeft.left, m_rcLeft.top + 2 * ( m_rcLeft.bottom - m_rcLeft.top ) / 4,
			20, 40, m_hMenuDC, 40, 160, SRCCOPY );
	}
	else
	{
		BitBlt( hDC, m_rcLeft.left, m_rcLeft.top + 2 * ( m_rcLeft.bottom - m_rcLeft.top ) / 4,
			20, 40, m_hMenuDC, 0, 160, SRCCOPY );
	}
	if ( m_Sel == 11 )
	{
		BitBlt( hDC, m_rcRight.left, m_rcRight.top + 2 * ( m_rcRight.bottom - m_rcRight.top ) / 4,
			20, 40, m_hMenuDC, 60, 160, SRCCOPY );
	}
	else
	{
		BitBlt( hDC, m_rcRight.left, m_rcRight.top + 2 * ( m_rcRight.bottom - m_rcRight.top ) / 4,
			20, 40, m_hMenuDC, 20, 160, SRCCOPY );
	}

	if ( m_RepNum == -1 )
	{
		BitBlt( hDC, m_rcLeft.right + 5, m_rcLeft.top + 2 * ( m_rcLeft.bottom - m_rcLeft.top ) / 4,
			50, 40, m_hMenuDC, 271, 160, SRCCOPY );
	}
	else
	{
		BitBlt( hDC, m_rcLeft.right + 36, m_rcLeft.top + 2 * ( m_rcLeft.bottom - m_rcLeft.top ) / 4,
			19, 40, m_hMenuDC, 80 + m_RepNum * 19, 160, SRCCOPY );
	}
}

// 選択中
void CMENU::Select( POINT pt )
{
	unsigned char before = m_Sel;

	m_Sel = 0;

	if ( PtInRect( &m_rcMenu, pt ) )
	{
		m_Sel = ( unsigned short )( pt.y - m_rcMenu.top ) / 40 + 1;
		if ( m_Sel == 2 )
		{
			m_Sel = 0;
			return;
		}
	}
	if ( PtInRect( &m_rcLeft, pt ) )
	{
		m_Sel = ( unsigned short )( pt.y - m_rcLeft.top ) / 40 + 5;
		if ( m_Sel != 7 && ( m_Level < 1 || m_Sel != 6 ) )
		{
			m_Sel = 0;
			return;
		}
	}
	if ( PtInRect( &m_rcRight, pt ) )
	{
		m_Sel = ( unsigned short )( pt.y - m_rcLeft.top ) / 40 + 9;
		if ( m_Sel != 11 && ( m_Level < 1 || m_Sel != 10 ) )
		{
			m_Sel = 0;
			return;
		}
	}

	if ( m_Sel != before && m_Sel )
		PlaySound( SELWAV, NULL, SND_FILENAME | SND_ASYNC );
}

// 選択
unsigned char CMENU::Click( void )
{
	switch ( m_Sel )
	{
		case 1:
			PlaySound( CLRWAV, NULL, SND_FILENAME | SND_ASYNC );
			return CR_BEGINNORMAL + m_MaskNum;

		case 3:
			PlaySound( CLRWAV, NULL, SND_FILENAME | SND_ASYNC );
			return CR_REPLAY0 + m_RepNum;

		case 4:
			PlaySound( CLRWAV, NULL, SND_FILENAME | SND_SYNC );
			return CR_ENDGAME;

		case 6:
			PlaySound( ERSWAV, NULL, SND_FILENAME | SND_ASYNC );
			if ( --m_MaskNum < 0 ) m_MaskNum = 0;
			break;

		case 7:
			PlaySound( ERSWAV, NULL, SND_FILENAME | SND_ASYNC );
			if ( --m_RepNum < -1 ) m_RepNum = -1;
			break;

		case 10:
			PlaySound( ERSWAV, NULL, SND_FILENAME | SND_ASYNC );
			if ( ++m_MaskNum > min( m_Level, MASKMAX ) ) m_MaskNum = min( m_Level, MASKMAX );
			break;

		case 11:
			PlaySound( ERSWAV, NULL, SND_FILENAME | SND_ASYNC );
			if ( ++m_RepNum > 9 ) m_RepNum = 9;
			break;
	}

	return CR_NOSTATUS;
}

// キーダウン
unsigned char CMENU::KeyDown( WPARAM key )
{
	switch ( key )
	{
		case VK_ESCAPE:
			return CR_ENDGAME;
	}

	return CR_NOSTATUS;
}

// デストラクタ
CMENU::~CMENU( void )
{
	SaveStatus();

	RelsSurface( m_hMenuDC, m_hMenuBm );
	RelsSurface( m_hDC, m_hBm );
}

// ステータス読み込み
void CGAME::LoadStatus( void )
{
	HANDLE hFile;
	DWORD dwRead;

	hFile = CreateFile( DATFILE, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( hFile == INVALID_HANDLE_VALUE ) return;

	SetFilePointer( hFile, 0, NULL, FILE_BEGIN );
	ReadFile( hFile, ( LPVOID )&m_Level, 1, &dwRead, NULL );

	CloseHandle( hFile );
}

// ステータス書き込み
void CGAME::SaveStatus( void )
{
	HANDLE hFile;
	DWORD dwWritten;

	hFile = CreateFile( DATFILE, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( hFile == INVALID_HANDLE_VALUE ) return;

	SetFilePointer( hFile, 0, NULL, FILE_BEGIN );
	WriteFile( hFile, ( LPCVOID )&m_Level, 1, &dwWritten, NULL );

	CloseHandle( hFile );
}


// EOF