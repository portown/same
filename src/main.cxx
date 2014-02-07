// main.cpp

#include "common.hxx"

// 定数の定義
#define CLSNAME ( LPCTSTR )"SAMEGAME"
#define WNDNAME ( LPCTSTR )"さめがめー"
#define WINX 640
#define WINY 480
#define GAMEX 320
#define GAMEY 480

// 関数のプロトタイプ宣言
LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );
BOOL InitApp( HINSTANCE );
BOOL InitInstance( HINSTANCE, int );
int Run( void );


// ==============================================
// 実装
// ==============================================

// エントリポイント
int WINAPI WinMain( HINSTANCE hCurInst, HINSTANCE hPrev, LPSTR lpCmd, int nCmd )
{
  int iRet;

  CreateMutex( NULL, FALSE, CLSNAME );
  if ( GetLastError() == ERROR_ALREADY_EXISTS ) return 0;

  if ( !SetCurDir() ) return 0;

  if ( !InitApp( hCurInst ) ) return 0;
  if ( !InitInstance( hCurInst, nCmd ) ) return 0;

  iRet = Run();

  return iRet;
}

// ウィンドウクラスの登録
BOOL InitApp( HINSTANCE hInst )
{
  WNDCLASSEX wc;

  wc.cbSize = sizeof( WNDCLASSEX );
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hbrBackground = ( HBRUSH )GetStockObject( WHITE_BRUSH );
  wc.hCursor = ( HCURSOR )LoadImage( hInst, MAKEINTRESOURCE( IDC_SAME ),
    IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED );
  wc.hIcon = ( HICON )LoadImage( hInst, MAKEINTRESOURCE( IDI_SAME ),
    IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED );
  wc.hIconSm = ( HICON )LoadImage( hInst, MAKEINTRESOURCE( IDI_SAME ),
    IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED );
  wc.hInstance = hInst;
  wc.lpfnWndProc = ( WNDPROC )WndProc;
  wc.lpszClassName = CLSNAME;
  wc.lpszMenuName = NULL;
  wc.style = CS_HREDRAW | CS_VREDRAW;

  if ( !RegisterClassEx( &wc ) )
    return FALSE;

  return TRUE;
}

// ウィンドウの作成
BOOL InitInstance( HINSTANCE hInst, int nCmd )
{
  HWND hWnd;
  SIZE szWnd;

  szWnd.cx = WINX + GetSystemMetrics( SM_CXFIXEDFRAME ) * 2;
  szWnd.cy = WINY + GetSystemMetrics( SM_CYFIXEDFRAME ) * 2 + GetSystemMetrics( SM_CYCAPTION );

  hWnd = CreateWindowEx( 0,
    CLSNAME,
    WNDNAME,
    WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
    ( GetSystemMetrics( SM_CXSCREEN ) - szWnd.cx ) / 2,
    ( GetSystemMetrics( SM_CYSCREEN ) - szWnd.cy ) / 2,
    szWnd.cx,
    szWnd.cy,
    NULL,
    NULL,
    hInst,
    NULL );

  if ( !hWnd ) return FALSE;

  ShowWindow( hWnd, nCmd );
  UpdateWindow( hWnd );

  return TRUE;
}

// メッセージ・ループ
int Run( void )
{
  MSG msg;

  while ( GetMessage( &msg, NULL, 0, 0L ) > 0 )
  {
    TranslateMessage( &msg );
    DispatchMessage( &msg );
  }

  return ( int )msg.wParam;
}

// ウィンドウプロシージャ
LRESULT CALLBACK WndProc( HWND hWnd, UINT msg, WPARAM wp, LPARAM lp )
{
  static HBITMAP s_hBackBm;
  static CGAME *s_pcGame = NULL;
  static HDC s_hBackDC;
  PAINTSTRUCT ps;
  POINT pt;
  HDC hDC;
  unsigned char ucRet;

  switch ( msg )
  {
    case WM_CREATE:
      InitSurface( s_hBackDC, s_hBackBm, WINX, WINY );
      s_pcGame = new CMENU( WINX, WINY );
      break;

    case WM_PAINT:
      hDC = BeginPaint( hWnd, &ps );
      PatBlt( s_hBackDC, 0, 0, WINX, WINY, BLACKNESS );
      s_pcGame->Draw( s_hBackDC );
      BitBlt( hDC, 0, 0, WINX, WINY, s_hBackDC, 0, 0, SRCCOPY );
      EndPaint( hWnd, &ps );
      break;

    case WM_MOUSEMOVE:
      pt.x = LOWORD( lp );
      pt.y = HIWORD( lp );
      s_pcGame->Select( pt );
      InvalidateRect( hWnd, NULL, FALSE );
      break;

    case WM_LBUTTONUP:
      ucRet = s_pcGame->Click();
      switch ( ucRet )
      {
        case CR_ENDGAME:
          DestroyWindow( hWnd );
          break;

        case CR_TITLEMENU:
          _DELETE( s_pcGame );
          s_pcGame = new CMENU( WINX, WINY );
          break;

        case CR_BEGINNORMAL:
        case CR_BEGINMASK1:
        case CR_BEGINMASK2:
        case CR_BEGINMASK3:
        case CR_BEGINMASK4:
          _DELETE( s_pcGame );
          s_pcGame = new CSAME( GAMEX, GAMEY, ucRet - CR_BEGINNORMAL );
          break;

        case CR_REPLAY:
        case CR_REPLAY0:
        case CR_REPLAY1:
        case CR_REPLAY2:
        case CR_REPLAY3:
        case CR_REPLAY4:
        case CR_REPLAY5:
        case CR_REPLAY6:
        case CR_REPLAY7:
        case CR_REPLAY8:
        case CR_REPLAY9:
          _DELETE( s_pcGame );
          s_pcGame = new CREPLAY( hWnd, GAMEX, GAMEY, ucRet - CR_REPLAY0 );
          break;

        case CR_CRITICALERROR:
          MessageBox( hWnd, "致命的なエラーが発生しました",
            "強制終了", MB_OK | MB_ICONEXCLAMATION );
          DestroyWindow( hWnd );
          break;
      }
      InvalidateRect( hWnd, NULL, FALSE );
      break;

    case WM_KEYDOWN:
      ucRet = s_pcGame->KeyDown( wp );
      switch ( ucRet )
      {
        case CR_TITLEMENU:
          _DELETE( s_pcGame );
          s_pcGame = new CMENU( WINX, WINY );
          break;

        case CR_BEGINNORMAL:
        case CR_BEGINMASK1:
        case CR_BEGINMASK2:
        case CR_BEGINMASK3:
        case CR_BEGINMASK4:
          _DELETE( s_pcGame );
          s_pcGame = new CSAME( GAMEX, GAMEY, ucRet - CR_BEGINNORMAL );
          break;

        case CR_ENDGAME:
          DestroyWindow( hWnd );
          break;

        default:
          return DefWindowProc( hWnd, msg, wp, lp );
      }
      InvalidateRect( hWnd, NULL, FALSE );
      break;

    case WM_TIMER:
      if ( wp == MINE_TIMER )
      {
        ( ( CREPLAY * )s_pcGame )->Replay();
      }
      break;

    case WM_DESTROY:
      _DELETE( s_pcGame );
      RelsSurface( s_hBackDC, s_hBackBm );
      PostQuitMessage( 0 );
      break;

    default:
      return DefWindowProc( hWnd, msg, wp, lp );
  }

  return 0L;
}


// EOF
