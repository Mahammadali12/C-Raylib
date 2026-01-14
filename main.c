#include <stdio.h>
#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>

#define WIDTH 800
#define HEIGHT 600


// gcc main.c -g -o bin/main -Wall -Wextra -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
Vector2 zero = {0,0};

typedef struct Circle
{
    Vector2 pos;
    Vector2 vel;
    Vector2 acc;
    float scale;
}Circle;

Circle circle = {.pos = {WIDTH/2,HEIGHT/2},.vel = {0,0},.acc = {0,0}, 30.0};
Vector2 gravity = {0,0.81};


void draw();
void update();
void updateCircle(Circle* c);
void applyGravity(Circle* c);
void checkBounds(Circle* c);
void applyWind(Circle* c, Vector2 wind);

int main ()
{
    InitWindow(WIDTH, HEIGHT, "raylib [core] example - basic window");
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        ClearBackground(BLACK);
            update();
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            {
                Vector2 wind = {1,0};
                applyWind(&circle, wind);
            }
        BeginDrawing();

            
            draw();


        EndDrawing();
        checkBounds(&circle);

    }

    CloseWindow();
    return 0;
}

void draw()
{
    DrawCircle(circle.pos.x,circle.pos.y,circle.scale,WHITE);
}

void update()
{
    updateCircle(&circle);
    
}

void updateCircle(Circle* c)
{
    applyGravity(c);

    c->vel = Vector2Add(c->vel,c->acc);
    c->pos = Vector2Add(c->pos,c->vel);
    c->acc = Vector2Multiply(c->acc,zero);



}

void applyGravity(Circle* c)
{
    Vector2 force = gravity;
    c->acc = Vector2Add(c->acc,force);
}

void checkBounds(Circle* c)
{
    if (c->pos.x >= WIDTH - c->scale || c->pos.x <= 0 + c->scale )
    {
        printf("touching horizontal\n");
        c->vel.x *= -1;
    }
    
    if (c->pos.y > HEIGHT - c->scale || c->pos.y <= 0 + c->scale)
    {
        printf("touching vertical\n");
        
        c->vel.y *= -1;
        // c->pos.y = HEIGHT;
    }
    
}

void applyWind(Circle* c, Vector2 wind)
{
    c->acc = Vector2Add(c->acc,wind);
}
