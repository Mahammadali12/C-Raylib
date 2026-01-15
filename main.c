#include <stdio.h>
#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>

#define WIDTH 800
#define HEIGHT 600

#define FRICTION_COEFFICIENT 0.7
#define DRAG_COEFFICIENT 0.38
// gcc main.c -g -o bin/main -Wall -Wextra -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

typedef struct Circle
{
    Vector2 pos;
    Vector2 vel;
    Vector2 acc;
    float scale;
    float mass;
}Circle;

Circle circle1 = {.pos = {WIDTH/2,HEIGHT/2},.vel = {0,0},.acc = {0,0}, 30.0, 10};
// Circle circle2 = {.pos = {WIDTH/2+100,HEIGHT/2},.vel = {0,0},.acc = {0,0}, 90.0,10};
Vector2 gravity = {0,9.81};


void draw();
void update();
void updateCircle(Circle* c);
void applyGravity(Circle* c, Vector2 gravity);
void checkBounds(Circle* c);
void applyForce(Circle* c, Vector2 force);
void applyFriction(Circle* c);
void contactEdge(Circle* c);
void applyDragForce(Circle* c);

int main ()
{
    InitWindow(WIDTH, HEIGHT, "raylib [core] example - basic window");
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
            if (IsKeyDown(KEY_A))
            {
                // Vector2 wind = {(circle1.pos.x-GetMouseX())/100,(circle1.pos.y-GetMouseY())/100};
                Vector2 wind = {-10.1,0};
                applyForce(&circle1, wind);
                // applyForce(&circle2, wind);
            }
            if (IsKeyDown(KEY_D))
            {
                // Vector2 wind = {(circle1.pos.x-GetMouseX())/100,(circle1.pos.y-GetMouseY())/100};
                Vector2 wind = {10.1,0};
                applyForce(&circle1, wind);
                // applyForce(&circle2, wind);
            }


        update();

        BeginDrawing();
        ClearBackground(BLACK);
            draw();
        EndDrawing();
        
        contactEdge(&circle1);
        checkBounds(&circle1);

    }

    CloseWindow();
    return 0;
}

void draw()
{
    DrawCircle(circle1.pos.x,circle1.pos.y,circle1.scale,GREEN);
    // DrawCircle(circle2.pos.x,circle2.pos.y,circle2.scale,WHITE);
}

void update()
{
    updateCircle(&circle1);
    // updateCircle(&circle2);
    
}

void updateCircle(Circle* c)
{
    applyGravity(c,gravity);
    applyDragForce(c);

    float dt = GetFrameTime();

    c->vel = Vector2Add(c->vel, Vector2Scale(c->acc,dt));
    c->pos = Vector2Add(c->pos, Vector2Scale(c->vel,dt));
    printf("y- %f\n",c->vel.y);

    c->acc = (Vector2){0,0};

}

void applyForce(Circle* c, Vector2 force)
{
    Vector2 f = (Vector2){force.x/c->mass, force.y/c->mass};
    c->acc = Vector2Add(c->acc,f);
}

void applyGravity(Circle* c, Vector2 gravity)
{
    gravity = Vector2Scale(gravity,c->mass);
    applyForce(c,gravity);

}

void applyFriction(Circle* c)
{
    float frictionMagnitude = FRICTION_COEFFICIENT * c->mass * gravity.y;   //mu * m * g
    Vector2 friction = c->vel;
    friction = Vector2Normalize(friction);
    friction = Vector2Negate(friction);
    friction = Vector2Scale(friction,frictionMagnitude);

    applyForce(c,friction);

}


void checkBounds(Circle* c)
{
    if (c->pos.x >= WIDTH - c->scale)
    {
        printf("touching horizontal\n");
        c->vel.x *= -1;
        c->pos.x = WIDTH - c->scale;
        // applyFriction(c);
    }

    if (c->pos.x <= 0 + c->scale)
    {
        printf("touching horizontal\n");
        c->vel.x *= -1;
        c->pos.x = 0 + c->scale;
        // applyFriction(c);
    }
    
    float loss = 0.9;
    if (c->pos.y > HEIGHT - c->scale)
    {
        printf("Bouncing\n");
        
        
        c->vel.y *= -loss;
        c->pos.y = HEIGHT - c->scale; 
        // applyFriction(c);
    }
    if (c->pos.y <= 0 + c->scale)
    {
        // printf("touching vertical\n");

        c->vel.y *= -loss;
        c->pos.y = 0 + c->scale;
        // applyFriction(c);
    }
}


void contactEdge(Circle* c)
{
    if (c->pos.y > HEIGHT - c->scale - 1 )
    {
        applyFriction(c);
        printf("FRICTION APPLIED\n");
    }
    
}

void applyDragForce(Circle* c)
{
    // -1/2 * surfaceArea * airDensity * dragCoefficient
    // float surfaceArea = PI * c->scale * c->scale; //surface area of halh circle;
    // float airDensity = 1.225;
    float speed = Vector2Length(c->vel);

    float dragMagnitude = 1 * speed * speed;

    Vector2 dragForce = Vector2Normalize(c->vel);
    dragForce = Vector2Negate(dragForce);

    dragForce = Vector2Scale(dragForce,dragMagnitude);
    printf("%f || %f\n",dragForce.x,dragForce.y);
    applyForce(c,dragForce);
}
