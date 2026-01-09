#include <stdio.h>
#include <raylib.h>
#include <stdlib.h>

#define WIDTH 800
#define HEIGHT 500


// gcc main.c -g -o main -Wall -Wextra -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
Vector2 circleVelocity = {1.0,1.0};
Vector2 circleCenter = {WIDTH/2+3,HEIGHT/2+31};

void moveCircle(Vector2 *circleCenter);
void checkEdges();
int main ()
{
    InitWindow(WIDTH, HEIGHT, "raylib [core] example - basic window");
    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(RAYWHITE);
            // printf("%f %f\n",circleCenter.x,circleCenter.y);
            DrawCircle(circleCenter.x,circleCenter.y,10.0,BLACK);
            moveCircle(&circleCenter);
            checkEdges();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}


void moveCircle(Vector2 *circleCenter)
{
    circleCenter->x += circleVelocity.x;
    circleCenter->y += circleVelocity.y;
}

void checkEdges()
{
    if (circleCenter.x == WIDTH || circleCenter.x == 0)
    {
        printf("PASSED WIDTH\n");
        printf("x - %f\n",circleCenter.x);
        circleVelocity.x *= -1;
        // exit(0);
    }
    if (circleCenter.y == HEIGHT || circleCenter.y == 0)
    {
        printf("PASSED HEIGHT\n");
        printf("y - %f\n",circleCenter.y);
        circleVelocity.y *= -1;
        // exit(0);
    }    
}
