/**
* Author: Ren Liao
* Assignment: Lunar Lander!
* Date due: [10/30/2026]
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "raylib.h"
#include "game/game.h"
#include "game/Entity.h"
using namespace std;

//Global Constants
constexpr int SCREEN_WIDTH = 1200;
constexpr int SCREEN_HEIGHT = 800;
constexpr int FPS = 120;

constexpr Vector2 ORIGIN = {SCREEN_WIDTH/2, SCREEN_HEIGHT/2};

constexpr int NUMBER_OF_GOOD_BLOCKS = 5;
constexpr int NUMBER_OF_BAD_TILES = 20;
constexpr float BLOCK_DIMENSION = 25.0f;
constexpr float TILE_DIMENSION = 25.0f;
constexpr float ACCELERATION_OF_GRAVITY = 50.0f;

constexpr float THRUST = 100.0f;
constexpr float DECELERATION =  50.0f;
constexpr float ACCELERATION = 75.0f;

//Global Variables
AppStatus gAppStatus = RUNNING;
float gPreviousTicks = 0.0f;
//float gTimeAccumulator = 0.0f;

Entity *gProtag = nullptr;
Entity *gTiles = nullptr;

//Function Declarations
void initialize();
void processInput();
void update();
void render();
void shutdown();

void initialize() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Kirby Lunar Lander");

    map<Direction, vector<int>> animationAtlas = {
        {UP,{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}},
        {RIGHT, {12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23}},
        {LEFT, {24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35}},
        {DOWN, {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47}}
    };

    float sizeRatio = 48.0f / 64.0f;

    gProtag = new Entity(
        {100.0f, 100.0f},
        {100.0f * sizeRatio, 100.0f},
        "assets/kirby_tornado.png",
        ATLAS,
        {4, 12},
        animationAtlas
    );
    gProtag->setColliderDimensions({gProtag->getScale().x / 3.0f, gProtag->getScale().y / 3.0f});
    gProtag->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});

    // --------------------------------------
    gTiles = new Entity[NUMBER_OF_BAD_TILES];
    float leftMostX = ORIGIN.x - (NUMBER_OF_BAD_TILES * TILE_DIMENSION)/2.0f;
    for (int i = 0; i<NUMBER_OF_BAD_TILES-1; i++) {
        gTiles[i].setTexture("assets/block.png");
        gTiles[i].setScale({TILE_DIMENSION, TILE_DIMENSION});
        gTiles[i].setColliderDimensions({TILE_DIMENSION, TILE_DIMENSION});
        gTiles[i].setPosition({
            leftMostX + i * TILE_DIMENSION,
            ORIGIN.y + TILE_DIMENSION
        });
    }
    gTiles[NUMBER_OF_BAD_TILES - 1].setTexture("assets/block.png");
    gTiles[NUMBER_OF_BAD_TILES - 1].setScale({ TILE_DIMENSION, TILE_DIMENSION });
    gTiles[NUMBER_OF_BAD_TILES - 1].setColliderDimensions({ TILE_DIMENSION, TILE_DIMENSION });
    gTiles[NUMBER_OF_BAD_TILES - 1].setPosition({
        ORIGIN.y + TILE_DIMENSION,
        ORIGIN.y - TILE_DIMENSION
    });

    SetTargetFPS(FPS);
}

void processInput() {
    gProtag->resetAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
    if (IsKeyDown(KEY_A)) gProtag->accelerateLeft(ACCELERATION);
    else if (IsKeyDown(KEY_D)) gProtag->accelerateRight(ACCELERATION);
    else {
        if (gProtag->getVelocity().x > 0.0) {gProtag->setAccelerationX(-DECELERATION);}
        else if (gProtag->getVelocity().x < 0.0) {gProtag->setAccelerationX(DECELERATION);}
    }


    if (IsKeyDown(KEY_W)) gProtag->thrust(THRUST);
    //else if (!IsKeyPressed(KEY_W)) gProtag->setAcceleration({gProtag->getAcceleration().x, ACCELERATION_OF_GRAVITY});

    //apply drifting

    if (getLength(gProtag->getMovement()) > 1.0f) {
        gProtag->normalizeMovement();
    }

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;

}

void update() {
    float ticks = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    //deltaTime += gTimeAccumulator;

    gProtag->update(deltaTime, gTiles, NUMBER_OF_BAD_TILES);

    for (int i = 0; i<NUMBER_OF_BAD_TILES; i++) {
        gTiles[i].update(deltaTime, nullptr, 0);
    }
   // gTimeAccumulator = deltaTime;
}

void render() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    gProtag->render();
    for (int i = 0; i<NUMBER_OF_BAD_TILES; i++) {
        gTiles[i].render();
    }
    EndDrawing();
}

void shutdown() {
    delete gProtag;
    delete[] gTiles;
    CloseWindow();
}

int main() {
    initialize();
    while (gAppStatus == RUNNING) {
        processInput();
        update();
        render();
    }

    shutdown();
    return 0;
}