#include "game.h"
void initSnake(void){
    int i;
    length = START_LENGTH;
    direction = DIR_RIGHT;
    nextDirection = DIR_RIGHT;
    for (i = 0; i < length; i++){
        snakeX[i] = (WIDTH / 2) - i;
        snakeY[i] = HEIGHT / 2;
    }
    oldTailX = snakeX[length - 1];
    oldTailY = snakeY[length - 1];
}

void moveSnake(void){
    int i;
    oldTailX = snakeX[length - 1];
    oldTailY = snakeY[length - 1];
    for (i = length - 1; i > 0; i--){
        snakeX[i] = snakeX[i - 1];
        snakeY[i] = snakeY[i - 1];
    }
    switch (direction){
        case DIR_UP:
            snakeY[0]--;
            break;
        case DIR_DOWN:
            snakeY[0]++;
            break;
        case DIR_LEFT:
            snakeX[0]--;
            break;
        case DIR_RIGHT:
            snakeX[0]++;
            break;
        default:
            break;
    }
}

void growSnake(void){
    if (length < MAX_LENGTH){
        snakeX[length] = oldTailX;
        snakeY[length] = oldTailY;
        length++;
    }
}
