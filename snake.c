#include "game.h"

void initSnake(void){
    int i;

    length = START_LENGTH;
    direction = DIR_RIGHT;
    nextDirection = DIR_RIGHT;
    for (i = 0; i < length; i++){
        snakeX[i] = WIDTH / 2 - i;
        snakeY[i] = HEIGHT / 2;
    }
}

void moveSnake(void){
    int i;
    for (i = length; i > 0; i--){
        snakeX[i] = snakeX[i - 1];
        snakeY[i] = snakeY[i - 1];
    }

    if (direction == DIR_UP)
        snakeY[0]--;
    else if (direction == DIR_DOWN)
        snakeY[0]++;
    else if (direction == DIR_LEFT)
        snakeX[0]--;
    else if (direction == DIR_RIGHT)
        snakeX[0]++;
}

void growSnake(void){
    if (length < MAX_LENGTH - 1)
        length++;
}

int checkWallCollision(void){
    if (snakeX[0] <= 0 || snakeX[0] >= WIDTH - 1)
        return 1;
    if (snakeY[0] <= 0 || snakeY[0] >= HEIGHT - 1)
        return 1;
    return 0;
}

int checkSelfCollision(void){
    int i;
    for (i = 1; i < length; i++)
        if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i])
            return 1;
    return 0;
}

void resetGame(void)
{
    initSnake();
    resetScore();
    gameOver = 0;
    paused = 0;
    generateFood();
}

void startGame(void)
{
    resetGame();
    gameState = STATE_PLAYING;
    SetTimer(hwndMain, GAME_TIMER_ID, (UINT)speed, NULL);
}

void gameTick(void)
{
    direction = nextDirection;
    moveSnake();

    if (checkWallCollision() || checkSelfCollision())
        endGame();
    else if (checkFoodEaten())
    {
        growSnake();
        updateScore();
        generateFood();
        if (gameOver)
            endGame();
    }
}

void endGame(void)
{
    KillTimer(hwndMain, GAME_TIMER_ID);
    gameOver = 1;
    updateHighScore();
    gameState = STATE_GAMEOVER;
}

void quitToMenu(void)
{
    KillTimer(hwndMain, GAME_TIMER_ID);
    updateHighScore();
    paused = 0;
    gameState = STATE_MENU;
}
