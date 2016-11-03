// main.cpp

#include "common.hxx"
#include "scene.h"
#include "menu.h"
#include "game.h"
#include "replay.h"
#include "gtips.h"

#include <ctime>


namespace
{
    constexpr auto CLSNAME = "main";
    constexpr auto WNDNAME = "‚³‚ß‚ª‚ß[";
    constexpr auto WINX    = 640;
    constexpr auto WINY    = 480;
    constexpr auto GAMEX   = 320;
    constexpr auto GAMEY   = 480;

    LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
    BOOL             InitApp(HINSTANCE);
    BOOL             InitInstance(HINSTANCE, int);
    int Run();
}


auto WINAPI WinMain(
    ::HINSTANCE const instance_handle,
    ::HINSTANCE,
    ::LPSTR,
    int const showing_command)
->int
{
    ::CreateMutex(NULL, FALSE, "jp.portown.same");
    if (::GetLastError() == ERROR_ALREADY_EXISTS) return 0;

    if (!::SetCurDir()) return 0;

    if (!InitApp(instance_handle)) return 0;
    if (!InitInstance(instance_handle, showing_command)) return 0;

    return Run();
}

namespace
{
    BOOL InitApp(HINSTANCE hInst)
    {
        WNDCLASSEX wc;

        wc.cbSize        = sizeof(WNDCLASSEX);
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;
        wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        wc.hCursor       = (HCURSOR)LoadImage(hInst, MAKEINTRESOURCE(IDC_SAME),
                                              IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
        wc.hIcon = (HICON)LoadImage(hInst, MAKEINTRESOURCE(IDI_SAME),
                                    IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
        wc.hIconSm = (HICON)LoadImage(hInst, MAKEINTRESOURCE(IDI_SAME),
                                      IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
        wc.hInstance     = hInst;
        wc.lpfnWndProc   = (WNDPROC)WndProc;
        wc.lpszClassName = CLSNAME;
        wc.lpszMenuName  = NULL;
        wc.style         = CS_HREDRAW | CS_VREDRAW;

        if (!RegisterClassEx(&wc))
            return FALSE;

        return TRUE;
    }

    BOOL InitInstance(HINSTANCE hInst, int nCmd)
    {
        HWND hWnd;
        SIZE szWnd;

        szWnd.cx = WINX + GetSystemMetrics(SM_CXFIXEDFRAME) * 2;
        szWnd.cy = WINY + GetSystemMetrics(SM_CYFIXEDFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION);

        hWnd = CreateWindowEx(0,
                              CLSNAME,
                              WNDNAME,
                              WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
                              (GetSystemMetrics(SM_CXSCREEN) - szWnd.cx) / 2,
                              (GetSystemMetrics(SM_CYSCREEN) - szWnd.cy) / 2,
                              szWnd.cx,
                              szWnd.cy,
                              NULL,
                              NULL,
                              hInst,
                              NULL);

        if (!hWnd) return FALSE;

        ShowWindow(hWnd, nCmd);
        UpdateWindow(hWnd);

        return TRUE;
    }

    int Run()
    {
        MSG msg;

        while (GetMessage(&msg, NULL, 0, 0L) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        return (int)msg.wParam;
    }

    LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
    {
        static Surface* backSurface = NULL;
        static GameScene* scene = NULL;
        PAINTSTRUCT    ps;
        POINT          pt;
        HDC            hDC;
        unsigned char  ucRet;

        switch (msg)
        {
            case WM_CREATE:
                backSurface = createSurface(WINX, WINY);
                scene = createGameScene(GSK_MENU, createGameSceneMenu(WINX, WINY));
                break;

            case WM_PAINT:
                hDC = BeginPaint(hWnd, &ps);
                PatBlt(backSurface->hDC, 0, 0, WINX, WINY, BLACKNESS);
                gameSceneDraw(scene, backSurface);
                BitBlt(hDC, 0, 0, WINX, WINY, backSurface->hDC, 0, 0, SRCCOPY);
                EndPaint(hWnd, &ps);
                break;

            case WM_MOUSEMOVE:
                pt.x = LOWORD(lp);
                pt.y = HIWORD(lp);
                gameSceneMouseMove(scene, pt);
                InvalidateRect(hWnd, NULL, FALSE);
                break;

            case WM_LBUTTONUP:
                ucRet = gameSceneMouseLDown(scene);
                switch (ucRet)
                {
                    case CR_ENDGAME:
                        DestroyWindow(hWnd);
                        break;

                    case CR_TITLEMENU:
                        destroyGameScene(scene);
                        scene = createGameScene(GSK_MENU, createGameSceneMenu(WINX, WINY));
                        break;

                    case CR_BEGINNORMAL:
                    case CR_BEGINMASK1:
                    case CR_BEGINMASK2:
                    case CR_BEGINMASK3:
                    case CR_BEGINMASK4:
                        destroyGameScene(scene);
                        scene = createGameScene(GSK_GAME, createGameSceneGame(GAMEX, GAMEY, ucRet - CR_BEGINNORMAL, (unsigned long)std::time(NULL)));
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
                        destroyGameScene(scene);
                        scene = createGameScene(GSK_REPLAY, createGameSceneReplay(hWnd, GAMEX, GAMEY, ucRet - CR_REPLAY0));
                        break;
                }
                InvalidateRect(hWnd, NULL, FALSE);
                break;

            case WM_KEYDOWN:
                ucRet = gameSceneKeyDown(scene, wp);
                switch (ucRet)
                {
                    case CR_TITLEMENU:
                        destroyGameScene(scene);
                        scene = createGameScene(GSK_MENU, createGameSceneMenu(WINX, WINY));
                        break;

                    case CR_BEGINNORMAL:
                    case CR_BEGINMASK1:
                    case CR_BEGINMASK2:
                    case CR_BEGINMASK3:
                    case CR_BEGINMASK4:
                        destroyGameScene(scene);
                        scene = createGameScene(GSK_GAME, createGameSceneGame(GAMEX, GAMEY, ucRet - CR_BEGINNORMAL, (unsigned long)std::time(NULL)));
                        break;

                    case CR_ENDGAME:
                        DestroyWindow(hWnd);
                        break;

                    default:
                        return DefWindowProc(hWnd, msg, wp, lp);
                }
                InvalidateRect(hWnd, NULL, FALSE);
                break;

            case WM_TIMER:
                if (wp == MINE_TIMER)
                {
                    gameSceneOnTimer(scene);
                }
                break;

            case WM_DESTROY:
                destroyGameScene(scene);
                scene = NULL;
                destroySurface(backSurface);
                backSurface = NULL;
                PostQuitMessage(0);
                break;

            default:
                return DefWindowProc(hWnd, msg, wp, lp);
        }

        return 0L;
    }
}
