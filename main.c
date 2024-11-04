#include "raylib.h"
#include "raymath.h"

const int BASE_SIZE = 50;
const int SCREEN_WIDTH = BASE_SIZE * 9;
const int SCREEN_HEIGHT = BASE_SIZE * 16;
const float PADDLE_CEILING_DISTANCE = (float) SCREEN_HEIGHT - 50.0f;
const float PADDLE_WIDTH = 80;
const float PADDLE_HEIGHT = 20;
const float BRICK_WIDTH = 78;
const float BRICK_HEIGHT = 20;
const float BOUNDARY_THICKNESS = 10.0f;
const float FPS_FONT_SIZE = 20.0f;
const float PLAYER_HORIZONTAL_SPEED = 175.0f;
const float BALL_SPEED = 300.0f;

typedef struct Paddle {
    Vector2 position;
} Paddle;

typedef struct Brick {
    Vector2 position;
    bool live;
} Brick;

typedef struct Circle {
    Vector2 center;
    float radius;
} Circle;

typedef struct Ball {
    Circle circle;
    Vector2 velocity;
} Ball;

Rectangle getRectangleFromPaddle(Paddle *paddle);

Rectangle getRectangleFromBoundary(Rectangle *rectangle);

Rectangle getRectangleFromBrick(Brick *brick);

void ResetGame(Paddle *paddle, Ball *ball, Brick *bricks, bool* startScreen, bool* gameOver, bool* gameFinish);

void CheckCollisionAndAdjust(Ball *ball, Rectangle rect);

void drawBoundary(Rectangle *rectangle);

void drawPaddle(Paddle *paddle);

void drawBall(Ball *ball);

void updatePaddle(Paddle *paddle, Rectangle *boundaryRectangle, float delta);

Ball InitBall(float x, float y, float vx, float vy, float r);

void updateBall(Ball *ball, Paddle *paddle, Rectangle *boundaryRectangle, Brick *bricks, float delta, bool *pBoolean,
                bool *pBoolean1);

void drawBricks(Brick *bricks);

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Brick Breaker");
    InitAudioDevice();
    Music music = LoadMusicStream("resources/background.mp3");
    SetTraceLogLevel(LOG_NONE);
    SetTargetFPS(60);

    Paddle paddle;
    Ball ball;
    struct Brick brick[20];
    Rectangle boundaryRectangle;
    float timePlayed = 0;
    bool startScreen = true;
    bool gameOver = false;
    bool gameFinish = false;

    ResetGame(&paddle, &ball, brick, &startScreen, &gameOver, &gameFinish);

    PlayMusicStream(music);
    while (!WindowShouldClose()) {
        UpdateMusicStream(music);
        timePlayed = GetMusicTimePlayed(music) / GetMusicTimeLength(music);
        if (timePlayed > 1.0f) timePlayed = 1.0f;
        float deltaTime = GetFrameTime();
        if (startScreen) {
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("Press SPACE to start game!", 80, SCREEN_HEIGHT / 2, 20, RAYWHITE);
            if (IsKeyDown(KEY_SPACE)) {
                startScreen = false;
            }
            EndDrawing();
            continue;
        }
        if (gameOver) {
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("Game Over, press R to start again", 40, SCREEN_HEIGHT / 2, 20, RAYWHITE);
            if (IsKeyDown(KEY_R)) {
                ResetGame(&paddle, &ball, brick, &startScreen, &gameOver, &gameFinish);
            }
            EndDrawing();
            continue;
        }
        if (gameFinish) {
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("You won, press R to start again", 45, SCREEN_HEIGHT / 2, 20, RAYWHITE);
            if (IsKeyDown(KEY_R)) {
                ResetGame(&paddle, &ball, brick, &startScreen, &gameOver, &gameFinish);
            }
            EndDrawing();
            continue;
        }

        updatePaddle(&paddle, &boundaryRectangle, deltaTime);
        updateBall(&ball, &paddle, &boundaryRectangle, brick, deltaTime, &gameFinish, &gameOver);
        BeginDrawing();
        ClearBackground(BLACK);
        DrawFPS(0, 0);
        drawBoundary(&boundaryRectangle);
        drawPaddle(&paddle);
        drawBall(&ball);
        drawBricks(brick);
        EndDrawing();
    }
    UnloadMusicStream(music);
    CloseWindow();

    return 0;
}


Rectangle getRectangleFromBoundary(Rectangle *boundaryRectangle) {
    Rectangle result;
    result.x = boundaryRectangle->x + BOUNDARY_THICKNESS;
    result.y = boundaryRectangle->y + BOUNDARY_THICKNESS;
    result.width = boundaryRectangle->width - (2 * BOUNDARY_THICKNESS);
    result.height = boundaryRectangle->height - BOUNDARY_THICKNESS;

    return result;
}

void CheckCollisionAndAdjust(Ball *ball, Rectangle rect) {
    // External collision detection (outside of rectangle)
    if (CheckCollisionCircleRec(ball->circle.center, ball->circle.radius, rect)) {
        // Check which side of the rectangle the circle is hitting and adjust velocity
        if (ball->circle.center.x < rect.x || ball->circle.center.x > rect.x + rect.width) {
            // Hitting left or right side of the rectangle
            ball->velocity.x *= -1; // Reverse horizontal direction
        }
        if (ball->circle.center.y < rect.y || ball->circle.center.y > rect.y + rect.height) {
            // Hitting top or bottom side of the rectangle
            ball->velocity.y *= -1; // Reverse vertical direction
        }
    }

    if ((ball->circle.center.x - ball->circle.radius > rect.x) &&
        (ball->circle.center.x + ball->circle.radius < rect.x + rect.width) &&
        (ball->circle.center.y - ball->circle.radius > rect.y) &&
        (ball->circle.center.y + ball->circle.radius < rect.y + rect.height)) {

        // Adjust to ensure the circle bounces off internal walls before fully escaping
        if (ball->circle.center.x - ball->circle.radius <= rect.x) {
            // Colliding with left internal wall
            ball->circle.center.x = rect.x + ball->circle.radius; // Prevent escape
            ball->velocity.x *= -1; // Reverse horizontal direction
        }
        if (ball->circle.center.x + ball->circle.radius >= rect.x + rect.width) {
            // Colliding with right internal wall
            ball->circle.center.x = rect.x + rect.width - ball->circle.radius; // Prevent escape
            ball->velocity.x *= -1; // Reverse horizontal direction
        }

        if (ball->circle.center.y - ball->circle.radius <= rect.y) {
            // Colliding with top internal wall
            ball->circle.center.y = rect.y + ball->circle.radius; // Prevent escape
            ball->velocity.y *= -1; // Reverse vertical direction
        }
        if (ball->circle.center.y + ball->circle.radius >= rect.y + rect.height) {
            // Colliding with bottom internal wall
            ball->circle.center.y = rect.y + rect.height - ball->circle.radius; // Prevent escape
            ball->velocity.y *= -1; // Reverse vertical direction
        }
    }
}

Rectangle getRectangleFromBrick(Brick *brick) {
    Rectangle result;
    result.x = brick->position.x - (BRICK_WIDTH / 2);
    result.y = brick->position.y - -(BRICK_HEIGHT / 2);
    result.width = BRICK_WIDTH;
    result.height = BRICK_HEIGHT;
    return result;
}

void drawBoundary(Rectangle *rectangle) {
    const float topMargin = 10.0f;
    const float margin = 5.0f;

    rectangle->x = margin;
    rectangle->y = FPS_FONT_SIZE;
    rectangle->width = (float) SCREEN_WIDTH - (2 * margin);
    rectangle->height = (float) SCREEN_HEIGHT - (1 * topMargin);
    DrawRectangleLinesEx(*rectangle, BOUNDARY_THICKNESS, RAYWHITE);
}

void drawPaddle(Paddle *paddle) {
    DrawRectangleRec(getRectangleFromPaddle(paddle), RAYWHITE);
}

void drawBall(Ball *ball) {
    DrawCircleV(ball->circle.center, 10, RAYWHITE);
}

Rectangle getRectangleFromPaddle(Paddle *paddle) {
    Rectangle result;
    result.x = paddle->position.x - (PADDLE_WIDTH / 2);
    result.y = paddle->position.y - (PADDLE_HEIGHT / 2);
    result.width = PADDLE_WIDTH;
    result.height = PADDLE_HEIGHT;

    return result;
}

void updateBall(Ball *ball, Paddle *paddle, Rectangle *boundaryRectangle, Brick *bricks, float delta, bool *winning,
                bool *lose) {
    CheckCollisionAndAdjust(ball, getRectangleFromPaddle(paddle));
    Rectangle innerBoundaryRectangle = getRectangleFromBoundary(boundaryRectangle);
    CheckCollisionAndAdjust(ball, innerBoundaryRectangle);

    int liveBrickCount = 20;
    for (int i = 0; i < 20; i++) {
        if (!bricks[i].live) {
            liveBrickCount -= 1;
            continue;
        }
        CheckCollisionAndAdjust(ball, getRectangleFromBrick(&bricks[i]));
        if (CheckCollisionCircleRec(ball->circle.center, 10.0f, getRectangleFromBrick(&bricks[i]))) {
            TraceLog(LOG_DEBUG, "Killed a brick");
            bricks[i].live = false;
        }
    }
    if (liveBrickCount == 0) {
        *winning = true;
        return;
    }

    ball->circle.center.x += ball->velocity.x * delta;
    ball->circle.center.y += ball->velocity.y * delta;

    if (ball->circle.center.y > (paddle->position.y + PADDLE_HEIGHT + 10)) {
        TraceLog(LOG_DEBUG, "YOU LOSE");
        *lose = true;
    }
}

void drawBricks(Brick *bricks) {
    for (int i = 0; i < 20; i++) {
        if (!bricks[i].live) continue;
        DrawRectangleRec(getRectangleFromBrick(&bricks[i]), RAYWHITE);
    }
}

void updatePaddle(Paddle *paddle, Rectangle *boundaryRectangle, float delta) {
    float paddleLeft = paddle->position.x - (PADDLE_WIDTH / 2);
    float paddleRight = paddle->position.x + (PADDLE_WIDTH / 2);

    bool paddleWithinLeftBounds = (paddleLeft >= (boundaryRectangle->x + BOUNDARY_THICKNESS));
    bool paddleWithinRightBounds =
            paddleRight <= (boundaryRectangle->x + boundaryRectangle->width - BOUNDARY_THICKNESS);

    if ((IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) && paddleWithinLeftBounds) {
        TraceLog(LOG_DEBUG, "Left key registered.");
        paddle->position.x -= PLAYER_HORIZONTAL_SPEED * delta;
    }
    if ((IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) && paddleWithinRightBounds) {
        TraceLog(LOG_DEBUG, "Right key registered.");
        paddle->position.x += PLAYER_HORIZONTAL_SPEED * delta;
    }
}

Ball InitBall(float x, float y, float vx, float vy, float r) {
    Vector2 position;
    Vector2 velocity;
    position.x = x;
    position.y = y;
    velocity.x = vx;
    velocity.y = vy;
    Circle circle;
    circle.center = position;
    circle.radius = r;
    Ball ball;
    ball.circle = circle;
    ball.velocity = velocity;
    return ball;
}

void ResetGame(Paddle *paddle, Ball *ball, Brick *bricks, bool* startScreen, bool* gameOver, bool* gameFinish) {
    paddle->position.x = (float) SCREEN_WIDTH / 2.0f;
    paddle->position.y = PADDLE_CEILING_DISTANCE;

    *ball = InitBall(
            (float) SCREEN_WIDTH / 2,
            (float) SCREEN_HEIGHT / 2,
            100,
            sqrtf(powf(BALL_SPEED, 2) - powf(ball->velocity.x, 2)),
            10
    );

    int yCount, xCount;
    yCount = 4;
    xCount = 5;
    float inlineMargin = 20.0f;
    float inlineSpacing = 5.0f;
    float ceilingMargin = 10.0f + FPS_FONT_SIZE + BOUNDARY_THICKNESS;
    for (int yItr = 0; yItr < yCount; yItr++) {
        for (int xItr = 0; xItr < xCount; xItr++) {
            struct Brick currentBrick;
            float brickX = inlineMargin + ((float) xItr * (BRICK_WIDTH + inlineSpacing)) - (BRICK_WIDTH / 2);
            float brickY = ceilingMargin + ((float) yItr * (inlineSpacing + BRICK_HEIGHT)) - (BRICK_HEIGHT / 2);
            currentBrick.position.x = brickX + BRICK_WIDTH;
            currentBrick.position.y = brickY + BRICK_HEIGHT;
            currentBrick.live = true;
            bricks[yItr * xCount + xItr] = currentBrick;
        }
    }

    float timePlayed = 0;

    *startScreen = true;
    *gameOver = false;
    *gameFinish = false;
}

