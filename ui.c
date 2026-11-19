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
static const char *getDifficultyName(void)
{
    if (difficulty == EASY)
    {
        return "Easy";
    }
    if (difficulty == HARD)
    {
        return "Hard";
    }
    return "Medium";
}

static void drawMenuBackground(HDC hdc)
{
    fillRect(hdc, 0, 0, WINDOW_W, WINDOW_H, CLR_MENU_BG);
}

static void drawTitle(HDC hdc, const char *title, const char *subtitle)
{
    drawTextBox(hdc, title, 0, 35, WINDOW_W, 115, 60, 1, CLR_YELLOW);
    drawTextBox(hdc, subtitle, 0, 115, WINDOW_W, 150, 18, 0, CLR_GRAY);
}

static void drawDecoration(HDC hdc)
{
    int i;
    int y = 455;
    int startX = (WINDOW_W - 10 * CELL) / 2;
    int headX = startX + 7 * CELL;
    int appleX = startX + 9 * CELL + CELL / 2;

    for (i = 0; i < 7; i++)
    {
        fillRounded(hdc, startX + i * CELL + 2, y + 2,
                    startX + (i + 1) * CELL - 2, y + CELL - 2,
                    10, CLR_SNAKE_BODY, CLR_SNAKE_BODY, 1);
    }

    fillRounded(hdc, headX + 1, y + 1, headX + CELL - 1, y + CELL - 1,
                12, CLR_SNAKE_HEAD, CLR_SNAKE_HEAD, 1);
    fillCircle(hdc, headX + 16, y + 7, 3, CLR_WHITE);
    fillCircle(hdc, headX + 16, y + 17, 3, CLR_WHITE);
    fillCircle(hdc, headX + 17, y + 7, 1, CLR_BLACK);
    fillCircle(hdc, headX + 17, y + 17, 1, CLR_BLACK);

    fillCircle(hdc, appleX, y + CELL / 2 + 1, 9, CLR_FOOD);
    fillCircle(hdc, appleX + 3, y + 3, 3, CLR_LEAF);
}

static void drawPanel(HDC hdc)
{
    fillRounded(hdc, 150, 100, 570, 450, 24, CLR_MENU_BG, CLR_YELLOW, 3);
}

static Button makeButton(int y, const char *label, int id, int highlight)
{
    Button b;

    b.x = (WINDOW_W - BTN_W) / 2;
    b.y = y;
    b.w = BTN_W;
    b.h = BTN_H;
    b.label = label;
    b.id = id;
    b.highlight = highlight;
    return b;
}

static int getButtons(Button *list)
{
    int n = 0;

    switch (gameState)
    {
        case STATE_MENU:
            list[n++] = makeButton(190, "Start Game", BTN_START, 0);
            list[n++] = makeButton(255, "Difficulty", BTN_DIFFICULTY, 0);
            list[n++] = makeButton(320, "High Score", BTN_HIGHSCORE, 0);
            list[n++] = makeButton(385, "Exit", BTN_EXIT, 0);
            break;

        case STATE_DIFFICULTY:
            list[n++] = makeButton(190, "Easy", BTN_EASY, difficulty == EASY);
            list[n++] = makeButton(255, "Medium", BTN_MEDIUM, difficulty == MEDIUM);
            list[n++] = makeButton(320, "Hard", BTN_HARD, difficulty == HARD);
            list[n++] = makeButton(385, "Back", BTN_BACK, 0);
            break;

        case STATE_HIGHSCORE:
            list[n++] = makeButton(385, "Back", BTN_BACK, 0);
            break;

        case STATE_PAUSED:
            list[n++] = makeButton(320, "Resume", BTN_RESUME, 0);
            list[n++] = makeButton(385, "Main Menu", BTN_MENU, 0);
            break;

        case STATE_GAMEOVER:
            list[n++] = makeButton(320, "Play Again", BTN_PLAY_AGAIN, 0);
            list[n++] = makeButton(385, "Main Menu", BTN_MENU, 0);
            break;

        default:
            break;
    }

    return n;
}
static void drawButton(HDC hdc, const Button *b)
{
    COLORREF fill = CLR_BTN;
    COLORREF textColor = CLR_WHITE;

    if (b->highlight)
    {
        fill = CLR_BTN_ACTIVE;
        textColor = CLR_DARK_TEXT;
    }
    else if (b->id == hoverId)
    {
        fill = CLR_BTN_HOVER;
    }

    fillRounded(hdc, b->x, b->y, b->x + b->w, b->y + b->h, 16,
                fill, CLR_BTN_BORDER, 2);
    drawTextBox(hdc, b->label, b->x, b->y, b->x + b->w, b->y + b->h,
                22, 1, textColor);
}

static void drawButtons(HDC hdc)
{
    Button list[4];
    int count = getButtons(list);
    int i;

    for (i = 0; i < count; i++)
    {
        drawButton(hdc, &list[i]);
    }
}

static int buttonAt(int x, int y)
{
    Button list[4];
    int count = getButtons(list);
    int i;

    for (i = 0; i < count; i++)
    {
        if (x >= list[i].x && x < list[i].x + list[i].w &&
            y >= list[i].y && y < list[i].y + list[i].h)
        {
            return list[i].id;
        }
    }
    return BTN_NONE;
}

static void doAction(int id)
{
    switch (id)
    {
        case BTN_START:
        case BTN_PLAY_AGAIN:
            startGame();
            break;

        case BTN_DIFFICULTY:
            gameState = STATE_DIFFICULTY;
            break;

        case BTN_HIGHSCORE:
            gameState = STATE_HIGHSCORE;
            break;

        case BTN_EXIT:
            DestroyWindow(hwndMain);
            break;

        case BTN_EASY:
            setDifficulty(EASY);
            gameState = STATE_MENU;
            break;

        case BTN_MEDIUM:
            setDifficulty(MEDIUM);
            gameState = STATE_MENU;
            break;

        case BTN_HARD:
            setDifficulty(HARD);
            gameState = STATE_MENU;
            break;

        case BTN_BACK:
        case BTN_MENU:
            quitToMenu();
            break;

        case BTN_RESUME:
            pauseGame();
            break;

        default:
            break;
    }

    hoverId = BTN_NONE;
    requestRedraw();
}

void drawBoard(HDC hdc)
{
    int x, y;
    int left, top;

    for (y = 0; y < HEIGHT; y++)
    {
        for (x = 0; x < WIDTH; x++)
        {
            left = x * CELL;
            top = HUD_HEIGHT + y * CELL;

            if (x == 0 || y == 0 || x == WIDTH - 1 || y == HEIGHT - 1)
            {
                fillRect(hdc, left, top, left + CELL, top + CELL, CLR_WALL);
                fillRect(hdc, left + 3, top + 3, left + CELL - 3, top + CELL - 3,
                         CLR_WALL_LIGHT);
            }
            else if ((x + y) % 2 == 0)
            {
                fillRect(hdc, left, top, left + CELL, top + CELL, CLR_GRASS_A);
            }
            else
            {
                fillRect(hdc, left, top, left + CELL, top + CELL, CLR_GRASS_B);
            }
        }
    }
}
