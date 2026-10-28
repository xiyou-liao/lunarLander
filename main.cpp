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
constexpr int SCREEN_WIDTH = 2000;
constexpr int SCREEN_HEIGHT = 2000;
constexpr int FPS = 120;

constexpr Vector2 ORIGIN = {SCREEN_WIDTH/2, SCREEN_HEIGHT/2};

constexpr int NUMBER_OF_GOOD_BLOCKS = 5;
constexpr int NUMBER_OF_BAD_BLOCKS = 20;
constexpr float BLOCK_DIMENSION = 50.0f;
constexpr float ACCELERATION_OF_GRAVITY = 250.0f;
//don't use ifxed timestep

//Global Variables
AppStatus gAppStatus = RUNNING;
float gPreviousTicks = 0.0f;
float gTimeAccumulator = 0.0f;

Entity *gProtag = nullptr;
Entity *gBlocks = nullptr;

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
        {250.0f * sizeRatio, 250.0f},
        "assets/kirby_tornado.png",
        ATLAS,
        {4, 12},
        animationAtlas
    );
    gProtag->setColliderDimensions({gProtag->getScale().x / 3.0f, gProtag->getScale().y / 3.0f});
    gProtag->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});


}

void processInput() {
    gProtag->resetMovement();
    if (IsKeyDown(KEY_A)) gProtag->accelerateLeft();
    else if (IsKeyDown(KEY_D)) gProtag->accelerateRight();

    if (IsKeyPressed(KEY_W)) gProtag->thrust();

    if (getLength(gProtag->getMovement()) > 1.0f) {
        gProtag->normalizeMovement();
    }

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() {
    float ticks = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    deltaTime += gTimeAccumulator;

    gProtag->update(deltaTime, gBlocks, NUMBER_OF_GOOD_BLOCKS);
    gTimeAccumulator = deltaTime;
}

void render() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    gProtag->render();

    EndDrawing();
}
int main() {

}