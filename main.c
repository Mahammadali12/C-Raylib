#include <stdio.h>
#include <raylib.h>
#include <stdlib.h>

#define WIDTH 1600
#define HEIGHT 1000


// gcc main.c -g -o bin/main -Wall -Wextra -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

typedef struct Circle
{
    Vector2 pos;
    Vector2 vel;
    float scale;
}Circle;

Circle circle = {.pos = {WIDTH/2,HEIGHT/2},.vel = {0,0},10.0};



void draw();


int main ()
{
    InitWindow(WIDTH, HEIGHT, "raylib [core] example - basic window");
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        BeginDrawing();
            
            
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            {
                draw();
            }

        EndDrawing();

    }

    CloseWindow();
    return 0;
}

void draw()
{
    DrawCircle(circle.pos.x,circle.pos.y,circle.scale,WHITE);
}