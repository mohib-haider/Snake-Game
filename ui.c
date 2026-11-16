#include "game.h"

#define CLR_GRASS_A     RGB(170, 215, 81)
#define CLR_GRASS_B     RGB(162, 209, 73)
#define CLR_WALL        RGB(74, 117, 44)
#define CLR_WALL_LIGHT  RGB(87, 138, 52)
#define CLR_HUD         RGB(55, 90, 35)
#define CLR_SNAKE_BODY  RGB(78, 124, 246)
#define CLR_SNAKE_HEAD  RGB(52, 95, 214)
#define CLR_FOOD        RGB(231, 71, 29)
#define CLR_LEAF        RGB(60, 160, 60)
#define CLR_MENU_BG     RGB(28, 45, 32)
#define CLR_WHITE       RGB(255, 255, 255)
#define CLR_BLACK       RGB(0, 0, 0)
#define CLR_YELLOW      RGB(255, 206, 84)
#define CLR_GRAY        RGB(170, 185, 170)
#define CLR_BTN         RGB(67, 160, 71)
#define CLR_BTN_HOVER   RGB(102, 187, 106)
#define CLR_BTN_BORDER  RGB(30, 90, 35)
#define CLR_BTN_ACTIVE  RGB(255, 193, 7)
#define CLR_DARK_TEXT   RGB(30, 30, 30)
#define CLR_RED_TEXT    RGB(255, 99, 71)

#define BTN_W   260
#define BTN_H   50

#define BTN_NONE        0
#define BTN_START       1
#define BTN_DIFFICULTY  2
#define BTN_HIGHSCORE   3
#define BTN_EXIT        4
#define BTN_EASY        5
#define BTN_MEDIUM      6
#define BTN_HARD        7
#define BTN_BACK        8
#define BTN_PLAY_AGAIN  9
#define BTN_MENU        10
#define BTN_RESUME      11

typedef struct{
    int x, y, w, h;
    const char *label;
    int id;
    int highlight;
}
Button;
static int hoverId = BTN_NONE;
static void requestRedraw(void){
    InvalidateRect(hwndMain, NULL, FALSE);
}
static RECT makeRect(int l, int t, int r, int b){
    RECT rc;
    rc.left = l;
    rc.top = t;
    rc.right = r;
    rc.bottom = b;
    return rc;
}

static void fillRect(HDC hdc, int l, int t, int r, int b, COLORREF color)
{
    HBRUSH brush = CreateSolidBrush(color);
    RECT rc = makeRect(l, t, r, b);

    FillRect(hdc, &rc, brush);
    DeleteObject(brush);
}

static void fillRounded(HDC hdc, int l, int t, int r, int b, int radius,
                        COLORREF fill, COLORREF border, int borderWidth)
{
    HBRUSH brush = CreateSolidBrush(fill);
    HPEN pen = CreatePen(PS_SOLID, borderWidth, border);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);

    RoundRect(hdc, l, t, r, b, radius, radius);

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(brush);
    DeleteObject(pen);
}

static void fillCircle(HDC hdc, int cx, int cy, int r, COLORREF color)
{
    HBRUSH brush = CreateSolidBrush(color);
    HPEN pen = CreatePen(PS_SOLID, 1, color);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);

    Ellipse(hdc, cx - r, cy - r, cx + r, cy + r);

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(brush);
    DeleteObject(pen);
}

static void drawTextIn(HDC hdc, const char *text, RECT *rect, int size,
                       int bold, COLORREF color, UINT format)
{
    HFONT font = CreateFontA(-size, 0, 0, 0, bold ? FW_BOLD : FW_NORMAL,
                             FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                             OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_SWISS,
                             "Segoe UI");
    HFONT oldFont = (HFONT)SelectObject(hdc, font);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, color);
    DrawTextA(hdc, text, -1, rect, format);

    SelectObject(hdc, oldFont);
    DeleteObject(font);
}

static void drawTextBox(HDC hdc, const char *text, int l, int t, int r, int b,
                        int size, int bold, COLORREF color)
{
    RECT rc = makeRect(l, t, r, b);

    drawTextIn(hdc, text, &rc, size, bold, color,
               DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}
