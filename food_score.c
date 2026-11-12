#include "game.h"

void generateFood(void)
{
    static int seeded = 0;
    int i;
    int onSnake;

    if (!seeded)
    {
        srand((unsigned int)time(NULL));
        seeded = 1;
    }

    if (length >= (WIDTH - 2) * (HEIGHT - 2))
    {
        gameOver = 1;
        return;
    }

    do
    {
        foodX = (rand() % (WIDTH - 2)) + 1;
        foodY = (rand() % (HEIGHT - 2)) + 1;

        onSnake = 0;
        for (i = 0; i < length; i++)
        {
            if (snakeX[i] == foodX && snakeY[i] == foodY)
            {
                onSnake = 1;
                break;
            }
        }
    } while (onSnake);
}
