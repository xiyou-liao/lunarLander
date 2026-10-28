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

//constexpr int NUMBER_OF_PLATFORMS = 5;
constexpr int NUMBER_OF_TOTAL_BLOCKS = 50;
constexpr float BLOCK_DIMENSION = 25.0f;
constexpr float TILE_DIMENSION = 25.0f;
constexpr float ACCELERATION_OF_GRAVITY = 15.0f;

constexpr float THRUST = 50.0f;
constexpr float DECELERATION =  25.0f;
constexpr float ACCELERATION = 50.0f;
//constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;
constexpr int MAX_FUEL = 400;
//Global Variables
AppStatus gAppStatus = RUNNING;
float gPreviousTicks = 0.0f;
//float gTimeAccumulator = 0.0f;
int fuel = MAX_FUEL;
bool stopUpdates = false;



Entity *gProtag = nullptr;
Entity *gTiles = nullptr;
//bool isMissionFailed = false;

//Function Declarations
void initialize();
void processInput();
void update();
void render();
void shutdown();
//void failMission();

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
        {75.0f * sizeRatio, 75.0f},
        "assets/kirby_tornado.png",
        ATLAS,
        {4, 12},
        PLAYER,
        animationAtlas
    );
    gProtag->setColliderDimensions({gProtag->getScale().x / 3.0f, gProtag->getScale().y / 3.0f});
    gProtag->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});

    // --------------------------------------

    gTiles = new Entity[NUMBER_OF_TOTAL_BLOCKS];
    float leftMostX = ORIGIN.x - (NUMBER_OF_TOTAL_BLOCKS * TILE_DIMENSION)/2.0f;
    for (int i = 0; i<NUMBER_OF_TOTAL_BLOCKS; i++) {
        gTiles[i].setTexture("assets/block.png");
        gTiles[i].setScale({TILE_DIMENSION, TILE_DIMENSION});
        gTiles[i].setColliderDimensions({TILE_DIMENSION, TILE_DIMENSION});
        gTiles[i].setEntityType(TILE);
    }
    vector<int> terrain =
        {1, 1, 2, 3, 4, 5, 4, 3, 3, 3, 2, 2, 1, 1, 0, 1, 2, 3, 4, 4, 4, 4, 5, 5, 6, 7, 8,
        9, 10, 10, 11, 12, 13, 14, 15, 14, 13, 12, 12, 11, 11, 10, 9, 9, 9, 8, 7, 6, 5, 4};

    for (size_t i = 0; i<NUMBER_OF_TOTAL_BLOCKS; i++) {
        gTiles[i].setPosition({
            i * TILE_DIMENSION,
            SCREEN_HEIGHT - terrain[i] * TILE_DIMENSION
        });
    }

    vector<int> platformLocations = {7, 14, 19, 44};
    for (size_t i = 0; i<platformLocations.size(); i++) {
        gTiles[platformLocations[i]].setTexture("assets/diamond_ore.png");
        gTiles[platformLocations[i]].setEntityType(PLATFORM);
    }
    SetTargetFPS(FPS);
}

void processInput() {
    gProtag->resetAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
    if (fuel > 0) {
        // stopUpdates = true;
        // return;
        if (IsKeyDown(KEY_W)) {
            gProtag->thrust(THRUST);
            if (fuel > 0 ) fuel --;
        }
        if (IsKeyDown(KEY_A)) {
            gProtag->accelerateLeft(ACCELERATION);
            if (fuel > 0 ) fuel --;
        }
        else if (IsKeyDown(KEY_D)) {
            gProtag->accelerateRight(ACCELERATION);
            if (fuel > 0 ) fuel --;
        }
    }
    else {
        if (gProtag->getVelocity().x > 0.0) {gProtag->setAccelerationX(-DECELERATION);}
        else if (gProtag->getVelocity().x < 0.0) {gProtag->setAccelerationX(DECELERATION);}
    }

    if (getLength(gProtag->getMovement()) > 1.0f) {
        gProtag->normalizeMovement();
    }

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;

}

void update() {
    float ticks = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    // deltaTime += gTimeAccumulator;
    // if (deltaTime < FIXED_TIMESTEP) {
    //     gTimeAccumulator = deltaTime;
    //     return;
    // }
    gProtag->update(deltaTime, gTiles, NUMBER_OF_TOTAL_BLOCKS);

    for (int i = 0; i<NUMBER_OF_TOTAL_BLOCKS; i++) {
        gTiles[i].update(deltaTime, nullptr, 0);
    }

    if (gProtag->getPosition().x < -50.0f || gProtag->getPosition().y < -50.0f ||
        gProtag->getPosition().x > SCREEN_WIDTH + 50.0 || gProtag->getPosition().y > SCREEN_HEIGHT + 50.0) {
        gAppStatus = TERMINATED;
    }

    // if (fuel == 0) {
    //     gProtag->failMission();
    // }
    if (gProtag->getMissionStatus() == FAIL) {
        cout << "Mission Failed" << endl;
        stopUpdates = true;
    }
    else if (gProtag->getMissionStatus() == SUCCESS) {
        cout << "Mission Success" << endl;
        stopUpdates = true;
    }
}

void render() {
    BeginDrawing();
    ClearBackground(BLACK);

    gProtag->render();
    for (int i = 0; i<NUMBER_OF_TOTAL_BLOCKS; i++) {
        gTiles[i].render();
    }
    DrawText(TextFormat("Fuel: %d", fuel), 100.0, 100.0, 60.0, WHITE);
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
        if (!stopUpdates) update();
        render();
    }

    shutdown();
    return 0;
}