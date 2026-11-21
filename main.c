#include "game.h"

int snakeX[MAX_LENGTH];
int snakeY[MAX_LENGTH];
int length = START_LENGTH;
int direction = DIR_RIGHT;
int nextDirection = DIR_RIGHT;
int foodX = 0,foodY = 0;
int score = 0,highScore = 0;
int difficulty = MEDIUM;
int speed = SPEED_MEDIUM;
int gameOver = 0;
int paused = 0;
int gameState = STATE_MENU;

int oldTailX = 0;
int oldTailY = 0;

HWND hwndMain = NULL;

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
    switch (msg){
        case WM_PAINT:{
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc;
            int w, h;
            HDC memDC;
            HBITMAP bitmap;
            HBITMAP oldBitmap;

            GetClientRect(hwnd, &rc);
            w = rc.right;
            h = rc.bottom;

            memDC = CreateCompatibleDC(hdc);
            bitmap = CreateCompatibleBitmap(hdc, w, h);
            oldBitmap = (HBITMAP)SelectObject(memDC, bitmap);

            drawFrame(memDC);
            BitBlt(hdc, 0, 0, w, h, memDC, 0, 0, SRCCOPY);
            SelectObject(memDC, oldBitmap);
            DeleteObject(bitmap);
            DeleteDC(memDC);
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_ERASEBKGND:
            return 1;

        case WM_TIMER:

            if (wParam == GAME_TIMER_ID && gameState == STATE_PLAYING){
                gameTick();
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;

        case WM_KEYDOWN:
            handleKeyDown((int)wParam);
            return 0;

        case WM_LBUTTONDOWN:
            handleMouseClick((short)LOWORD(lParam), (short)HIWORD(lParam));
            return 0;

        case WM_MOUSEMOVE:
            handleMouseMove((short)LOWORD(lParam), (short)HIWORD(lParam));
            return 0;

        case WM_DESTROY:
            KillTimer(hwnd, GAME_TIMER_ID);
            PostQuitMessage(0);
            return 0;

        default:
            break;
    }

    return DefWindowProcA(hwnd, msg, wParam, lParam);
}
