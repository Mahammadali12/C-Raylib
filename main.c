#include <stdio.h>
#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>

#define WIDTH 1500
#define HEIGHT 600

// Physics / units
#define PPM  50.0f              // pixels per meter (tune to taste)
#define WIDTH_M  ((float)WIDTH / PPM)
#define HEIGHT_M ((float)HEIGHT / PPM)



#define AIR_DENSITY 1.225f       // kg/m^3 (at sea level)
#define LIFT_COEFFICIENT 1.5f    // dimensionless
#define FRICTION_COEFFICIENT 0.7f
#define DRAG_COEFFICIENT 0.38f

// #define EPSILON 1e-4f
#define REST_VEL_THRESHOLD 0.1f   // m/s: impact speed below this -> treat as rest
#define HORIZONTAL_VEL_SLEEP 0.01f // m/s: horizontal sleep threshold

float totalTime = 0.0f;

typedef struct Circle
{
    Vector2 pos;   // meters
    Vector2 vel;   // m/s
    Vector2 acc;   // m/s^2 (accumulated each frame)
    float scale;   // radius in meters
    float mass;    // kg
    int onGround;  // boolean: 1 if resting contact
    float AngleOfAttack; // in radians
} Circle;

// Initialize physics state in meters
Circle circle1 = {
    .pos = { (float)WIDTH/2 / PPM, (float)HEIGHT/2 / PPM }, // meters
    .vel = { 0, 0 },
    .acc = { 0, 0 },
    .scale = 0.3f, // meters (was 3.0 before — adjust as you like)
    .mass = 3.0f,
    .onGround = 0
};

const Vector2 GRAVITY = { 0.0f, 9.81f }; // m/s^2 down

void draw();
void update();
void updateCircle(Circle* c);
void applyGravity(Circle* c);
void checkBounds(Circle* c);
void applyForce(Circle* c, Vector2 force); // force in Newtons
void applyFrictionTRASH(Circle* c);
void applyFriction(Circle* c, float dt);
void applyDragForce(Circle* c);
void applyLiftForce(Circle* c);
void applyInducedDragForce(Circle* c);



int main(void)
{
    InitWindow(WIDTH, HEIGHT, "Physics: resting contact demo");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // Example input forces (forces are in Newtons)
        if (IsKeyDown(KEY_A)) applyForce(&circle1, (Vector2){ -95.1f, 0.0f });
        if (IsKeyDown(KEY_D)) applyForce(&circle1, (Vector2){  95.1f, 0.0f });

        if (IsKeyDown(KEY_W))
        {
            circle1.AngleOfAttack += PI/180; // increase by 1 degree in radians
        }

        if (IsKeyDown(KEY_S))
        {
            circle1.AngleOfAttack -= PI/180; // decrease by 1 degree in radians
        }

        

        update();

        BeginDrawing();
            ClearBackground(BLACK);
            draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

void draw()
{
    // Convert meters -> pixels for rendering
    DrawCircle((int)(circle1.pos.x * PPM), (int)(circle1.pos.y * PPM), circle1.scale * PPM, GREEN);
}

void update()
{
    updateCircle(&circle1);
}

void updateCircle(Circle* c)
{
    float dt = GetFrameTime();
    totalTime += dt;
    if (dt <= 0) return;

    // Apply gravity only if NOT in resting contact
    applyGravity(c);

    // Aerodynamic drag (uses velocity in m/s and area in m^2)
    applyDragForce(c);

    applyFriction(c, dt);


    applyLiftForce(c);

    applyInducedDragForce(c);

    // Integrate (semi-implicit Euler)
    c->vel = Vector2Add(c->vel, Vector2Scale(c->acc, dt));   // v += a * dt
    c->pos = Vector2Add(c->pos, Vector2Scale(c->vel, dt));   // x += v * dt

    // Ground/contact/collision handling adjusts pos/vel and sets onGround
    checkBounds(c);

    // Horizontal sleeping: small horizontal vel -> zero to avoid jitter
    if (c->onGround)
    {
        if (fabsf(c->vel.x) < HORIZONTAL_VEL_SLEEP) c->vel.x = 0.0f;
    }

    // Debug (in meters / m/s)
    printf("ACC: x %f || y %f\n", c->acc.x, c->acc.y);
    printf("VEL: x %f || y %f\n", c->vel.x, c->vel.y);
    printf("POS: x %f || y %f\n", c->pos.x, c->pos.y);
    printf("Total Time: %f\n", totalTime);
    printf("-----------------------\n");

    // Clear accumulated accelerations (forces must be re-applied each frame)
    c->acc = Vector2Zero();

}

void applyForce(Circle* c, Vector2 force)
{
    // force in Newtons -> acceleration (m/s^2)
    Vector2 a = { force.x / c->mass, force.y / c->mass };
    c->acc = Vector2Add(c->acc, a);
}

void applyGravity(Circle* c)
{
    // Only add gravity if not resting
    if (!c->onGround)
    {
        c->acc = Vector2Add(c->acc, GRAVITY);
    }
}

void applyFrictionTRASH(Circle* c)
{
    // Apply kinetic friction along horizontal tangent when contacting ground
    float normal = c->mass * GRAVITY.y; // N = m * g (approx, flat ground)
    float frictionMagnitude = FRICTION_COEFFICIENT * normal; // N

    float vx = c->vel.x;
    float speed = fabsf(vx);
    if (speed < EPSILON) return;

    Vector2 friction = { -vx / speed * frictionMagnitude / c->mass, 0.0f };
    // friction is added as acceleration (F/m)
    // c->acc = Vector2Add(c->acc, friction);
    printf("Friction applied TRASH: %f\n", friction.x);
    
}

void applyFriction(Circle* c, float dt)
{

    // Check if on ground and kinetic  friction should be applied
    // After applying gravity and drag to acc
    Vector2 predicted_vel = Vector2Add(c->vel, Vector2Scale(c->acc, dt));
    Vector2 predicted_pos = Vector2Add(c->pos, Vector2Scale(predicted_vel, dt));
    // printf("Predicted POS Y: %f\n", predicted_pos.y + c->scale);
    // printf("HEIGHT_M: %f\n", HEIGHT_M);
    // printf("onGround before check: %d\n", c->onGround);
    
    if (predicted_pos.y + c->scale >= HEIGHT_M)
    {
        c->onGround = 1;
        // printf("Predicted to be on ground\n");    
    }
    else
    {
        // printf("Predicted to be in air\n");
        c->onGround = 0;
        return;
    }
    
    if (c->onGround == 1)
    {
        float frictionMagnitude = FRICTION_COEFFICIENT * c->mass * GRAVITY.y;   //mu * m * g

        float vx = c->vel.x;
        float speed = fabsf(vx);
        if (speed < EPSILON)
        {
            printf("No friction applied, speed too low\n");
            return;

        } 

        Vector2 friction = { -vx / speed * frictionMagnitude , 0.0f }; // in Newtons
        // friction is added as acceleration (F/m) 

        Vector2 v_after_friction = Vector2Add(c->vel, Vector2Scale(friction, dt / c->mass));

        if ((c->vel.x > 0 && v_after_friction.x < 0) || (c->vel.x < 0 && v_after_friction.x > 0))
        {
            printf("Friction would reverse velocity, adjusting to stop\n");
            friction.x = -c->vel.x * c->mass / dt; // total stop
        }
        

        applyForce(c,friction);
        printf("Friction applied : %f\n", friction.x);
        // printf("Friction applied : %f\n", friction.y);
    }
}

void checkBounds(Circle* c)
{
    // Horizontal walls (world in meters)
    if (c->pos.x + c->scale > WIDTH_M)
    {
        c->pos.x = WIDTH_M - c->scale;
        c->vel.x *= -0.5f; // simple bounce loss
    }
    if (c->pos.x - c->scale < 0.0f)
    {
        c->pos.x = c->scale;
        c->vel.x *= -0.5f;
    }

    // Ground (y increases downward)
    float groundY = HEIGHT_M;
    if (c->pos.y + c->scale >= groundY)
    {
        // We penetrated the ground; correct position to sit on top
        c->pos.y = groundY - c->scale;

        float impactSpeed = c->vel.y;

        if (fabsf(impactSpeed) > REST_VEL_THRESHOLD)
        {
            // Bounce: still a collision
            const float loss = 0.9f;
            c->vel.y = -impactSpeed * loss;
            c->onGround = 0;
        }
        else
        {
            // Resting contact (static)
            c->vel.y = 0.0f;
            c->acc.y = 0.0f;
            c->onGround = 1;

            // Apply friction while resting (horizontal only)
            // applyFrictionTRASH(c);
            // applyFriction(c);
        }
    }
    else
    {
        // Not touching ground
        c->onGround = 0;
    }

    // Ceiling
    if (c->pos.y - c->scale < 0.0f)
    {
        c->pos.y = c->scale;
        c->vel.y *= -0.5f;
    }
}

void applyDragForce(Circle* c)
{
    float speed = Vector2Length(c->vel);
    if (speed < EPSILON) return; // nothing to do

    // Projected area of a circle: A = π r^2  (r in meters)
    float area = PI * c->scale * c->scale;
    float rho = 1.225f;

    float dragMag = 0.5f * rho * DRAG_COEFFICIENT * area * speed * speed; // Newtons

    // Direction opposite velocity
    Vector2 dir = Vector2Scale(Vector2Normalize(c->vel), -1.0f);
    Vector2 dragForce = Vector2Scale(dir, dragMag);

    // apply as acceleration (F/m)
    applyForce(c, dragForce);
}


void applyLiftForce(Circle* c)
{
    // Optional: implement lift force if desired
    // printf("Lift Force applied:\n");

    if (c->AngleOfAttack > PI/4)
    {
        c->AngleOfAttack = PI/4;
    }
    else if (c->AngleOfAttack < -PI/9)
    {
        c->AngleOfAttack = -PI/9; 
    }
    
    // in degrees for debugging
    printf("Angle of Attack (degrees): %f\n", c->AngleOfAttack * (180.0f / PI));

    float speed = Vector2Length(c->vel);
    if (speed < EPSILON) return; // nothing to do
    float area = PI * c->scale * c->scale;
    float AoA =  c->AngleOfAttack; 
    float C_L = LIFT_COEFFICIENT * sinf(2 * AoA);
    float liftMagnitude = 0.5f * AIR_DENSITY * C_L * area * speed * speed; // Newtons

    Vector2 liftDir = Vector2Normalize(c->vel);
    liftDir = (Vector2){ -liftDir.y, liftDir.x }; // Perpendicular to velocity
    Vector2 liftForce = Vector2Scale(liftDir, liftMagnitude);
    printf("Lift Magnitude: %f\n", liftMagnitude);
    applyForce(c, liftForce);

}

void applyInducedDragForce(Circle* c)
{
    // Optional: implement induced drag if desired
    float liftCoefficient = LIFT_COEFFICIENT * sinf(2 * c->AngleOfAttack);
    float k = 1.0f / (PI * 1.0f * 0.9f); // assuming aspect ratio of 1.0 and efficiency factor of 0.9
    float inducedDragCoefficient = k * liftCoefficient * liftCoefficient; // assuming aspect ratio of 1.0 and efficiency factor of 0.9
    float speed = Vector2Length(c->vel);
    if (speed < EPSILON) return; // nothing to do
    float area = PI * c->scale * c->scale;
    float inducedDragMagnitude = 0.5f * AIR_DENSITY * inducedDragCoefficient * area * speed * speed; // Newtons
    Vector2 dragDir = Vector2Normalize(c->vel);
    dragDir = Vector2Negate(dragDir); 
    Vector2 inducedDragForce = Vector2Scale(dragDir, inducedDragMagnitude);
    printf("Induced Drag Magnitude: %f\n", inducedDragMagnitude);
    applyForce(c, inducedDragForce);
}