#include "game.h"

int snakeX[MAX_LENGTH];
int snakeY[MAX_LENGTH];
int length = START_LENGTH;
int direction = DIR_RIGHT;
int nextDirection = DIR_RIGHT;

int foodX = 0;
int foodY = 0;

int score = 0;
int highScore = 0;

int difficulty = MEDIUM;
int speed = SPEED_MEDIUM;
int gameOver = 0;
int paused = 0;
int gameState = STATE_MENU;

HWND hwndMain = NULL;

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_PAINT){
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP bitmap = CreateCompatibleBitmap(hdc, WINDOW_W, WINDOW_H);
        HBITMAP old = (HBITMAP)SelectObject(memDC, bitmap);

        drawFrame(memDC);
        BitBlt(hdc, 0, 0, WINDOW_W, WINDOW_H, memDC, 0, 0, SRCCOPY);

        SelectObject(memDC, old);
        DeleteObject(bitmap);
        DeleteDC(memDC);
        EndPaint(hwnd, &ps);
        return 0;
    }
    if (msg == WM_ERASEBKGND)
        return 1;
    if (msg == WM_TIMER && gameState == STATE_PLAYING){
        gameTick();
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }
    if (msg == WM_KEYDOWN){
        handleKeyDown((int)wParam);
        return 0;
    }
    if (msg == WM_LBUTTONDOWN){
        handleMouseClick(LOWORD(lParam), HIWORD(lParam));
        return 0;
    }
    if (msg == WM_MOUSEMOVE){
        handleMouseMove(LOWORD(lParam), HIWORD(lParam));
        return 0;
    }
    if (msg == WM_DESTROY){
        updateHighScore();
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR cmdLine, int nCmdShow){
	(void)hPrev;
	(void)cmdLine;
    WNDCLASSA wc;
    MSG msg;
    DWORD style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    RECT size = {0, 0, WINDOW_W, WINDOW_H};

    memset(&wc, 0, sizeof(wc));
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursorA(NULL, IDC_ARROW);
    wc.lpszClassName = "SnakeGame";
    RegisterClassA(&wc);

    AdjustWindowRect(&size, style, FALSE);
    hwndMain = CreateWindowA("SnakeGame", "Snake Game", style,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             size.right - size.left, size.bottom - size.top,
                             NULL, NULL, hInstance, NULL);
    if (hwndMain == NULL)
        return 1;

    loadHighScore();
    ShowWindow(hwndMain, nCmdShow);

    while (GetMessageA(&msg, NULL, 0, 0))
        DispatchMessageA(&msg);

    return 0;
}
