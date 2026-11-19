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

int checkFoodEaten(void)
{
    if (snakeX[0] == foodX && snakeY[0] == foodY)
    {
        return 1;
    }
    return 0;
}

void updateScore(void)
{
    score += POINTS_PER_APPLE;
}

void resetScore(void)
{
    score = 0;
}

void loadHighScore(void)
{
    FILE *fp = fopen(HIGHSCORE_FILE, "r");
    if (fp == NULL)
    {
        highScore = 0;
        return;
    }
    if (fscanf(fp, "%d", &highScore) != 1)
    {
        highScore = 0;
    }
    if (highScore < 0)
    {
        highScore = 0;
    }
    fclose(fp);
}

void saveHighScore(void)
{
    FILE *fp = fopen(HIGHSCORE_FILE, "w");

    if (fp == NULL)
    {
        return;
    }
    fprintf(fp, "%d", highScore);
    fclose(fp);
}

void updateHighScore(void)
{
    if (score > highScore)
    {
        highScore = score;
        saveHighScore();
    }
}
