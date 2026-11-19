#include "game.h"

#define CLR_GRASS_A     RGB(170, 215, 81)
#define CLR_GRASS_B     RGB(162, 209, 73)
#define CLR_WALL        RGB(74, 117, 44)
#define CLR_HUD         RGB(55, 90, 35)
#define CLR_BODY        RGB(78, 124, 246)
#define CLR_HEAD        RGB(52, 95, 214)
#define CLR_FOOD        RGB(231, 71, 29)
#define CLR_DARK        RGB(28, 45, 32)
#define CLR_WHITE       RGB(255, 255, 255)
#define CLR_YELLOW      RGB(255, 206, 84)
#define CLR_GRAY        RGB(170, 185, 170)
#define CLR_RED         RGB(255, 99, 71)
#define CLR_BTN         RGB(67, 160, 71)
#define CLR_BTN_HOT     RGB(102, 187, 106)

#define SHAPE_BOX       0
#define SHAPE_ROUND     1
#define SHAPE_CIRCLE    2

#define ALIGN_CENTER    (DT_CENTER | DT_VCENTER | DT_SINGLELINE)
#define REDRAW()        InvalidateRect(hwndMain, NULL, FALSE)

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

#define BTN_W           260
#define BTN_H           50
#define BTN_X           ((WINDOW_W - BTN_W) / 2)

typedef struct{
    int state,id,y;
    const char *label;
} Button;

static const Button buttons[] ={
    {STATE_MENU,       190, "Start Game", BTN_START},
    {STATE_MENU,       255, "Difficulty", BTN_DIFFICULTY},
    {STATE_MENU,       320, "High Score", BTN_HIGHSCORE},
    {STATE_MENU,       385, "Exit",       BTN_EXIT},
    {STATE_DIFFICULTY, 190, "Easy",       BTN_EASY},
    {STATE_DIFFICULTY, 255, "Medium",     BTN_MEDIUM},
    {STATE_DIFFICULTY, 320, "Hard",       BTN_HARD},
    {STATE_DIFFICULTY, 385, "Back",       BTN_BACK},
    {STATE_HIGHSCORE,  385, "Back",       BTN_BACK},
    {STATE_PAUSED,     320, "Resume",     BTN_RESUME},
    {STATE_PAUSED,     385, "Main Menu",  BTN_MENU},
    {STATE_GAMEOVER,   320, "Play Again", BTN_PLAY_AGAIN},
    {STATE_GAMEOVER,   385, "Main Menu",  BTN_MENU}
};

#define BUTTON_COUNT    ((int)(sizeof(buttons) / sizeof(buttons[0])))

static const char *levelName[] = {"", "Easy", "Medium", "Hard"};
static int hoverId = BTN_NONE;
static void shape(HDC hdc, int kind, int l, int t, int r, int b, COLORREF color){
    HBRUSH brush = CreateSolidBrush(color);
    HPEN pen = CreatePen(PS_SOLID, 1, color);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    if (kind == SHAPE_ROUND)
        RoundRect(hdc, l, t, r, b, 14, 14);
    else if (kind == SHAPE_CIRCLE)
        Ellipse(hdc, l, t, r, b);
    else
        Rectangle(hdc, l, t, r, b);

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(brush);
    DeleteObject(pen);
}

static void drawText(HDC hdc, const char *text, int l, int t, int r, int b,
                     int size, COLORREF color, UINT format){
    RECT rc;
    HFONT font = CreateFontA(-size, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                             CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                             DEFAULT_PITCH | FF_SWISS, "Segoe UI");
    HFONT oldFont = (HFONT)SelectObject(hdc, font);
    rc.left = l;
    rc.top = t;
    rc.right = r;
    rc.bottom = b;
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, color);
    DrawTextA(hdc, text, -1, &rc, format);
    SelectObject(hdc, oldFont);
    DeleteObject(font);
}

static void drawButtons(HDC hdc){
    int i;
    for (i = 0; i < BUTTON_COUNT; i++){
        if (buttons[i].state != gameState)
            continue;
        shape(hdc, SHAPE_ROUND, BTN_X, buttons[i].y, BTN_X + BTN_W, buttons[i].y + BTN_H,
              buttons[i].id == hoverId ? CLR_BTN_HOT : CLR_BTN);
        drawText(hdc, buttons[i].label, BTN_X, buttons[i].y, BTN_X + BTN_W,
                 buttons[i].y + BTN_H, 22, CLR_WHITE, ALIGN_CENTER);
    }
}

static int buttonAt(int x, int y){
    int i;
    for (i = 0; i < BUTTON_COUNT; i++){
        if (buttons[i].state == gameState &&
            x >= BTN_X && x < BTN_X + BTN_W &&
            y >= buttons[i].y && y < buttons[i].y + BTN_H)
            return buttons[i].id;
    }
    return BTN_NONE;
}
