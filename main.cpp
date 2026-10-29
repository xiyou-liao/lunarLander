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

constexpr int NUMBER_OF_TOTAL_BLOCKS = 52;
constexpr float BLOCK_DIMENSION = 25.0f;
constexpr float TILE_DIMENSION = 25.0f;
constexpr float ACCELERATION_OF_GRAVITY = 15.0f;

constexpr float THRUST = 75.0f;
constexpr float ACCELERATION = 50.0f;
constexpr int MAX_FUEL = 300;

//Global Variables
AppStatus gAppStatus = RUNNING;
float gPreviousTicks = 0.0f;
float fuel = MAX_FUEL;
float fuelConsumption = 0.2f;
bool kirbyExplode = false;
bool missionSuccess = false;
float gAngle = 0.0f;

Entity *gProtag = nullptr;
Entity *gTiles = nullptr;

Texture2D gExplosionTexture;
Vector2 gScaleExplosion = {150.0f, 100.0f};
Vector2 gMissionStatusPosition = {400.0f, ORIGIN.y - 100.0f};

//Function Declarations
void initialize();
void processInput();
void update();
void render();
void shutdown();
void renderObject();

void renderObject(const Texture2D* texture, const Vector2* position, const Vector2* scale) {
    Rectangle textureArea = {
        0.0f, 0.0f,
        static_cast<float>(texture->width), static_cast<float>(texture->height)
    };

    Rectangle destinationArea = {
        position->x, position->y,
        static_cast<float>(scale->x), static_cast<float>(scale->y)
    };

    Vector2 originOffset = {
        static_cast<float>(scale->x) / 2.0f, static_cast<float>(scale->y) / 2.0f
    };

    DrawTexturePro(*texture, textureArea, destinationArea, originOffset, gAngle, WHITE);
}

void initialize() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Kirby Lunar Lander");

    map<Direction, vector<int>> animationAtlas = { //atlas of kirby animation sprite
        {UP,{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}},
        {RIGHT, {12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23}},
        {LEFT, {24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35}},
        {DOWN, {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47}}
    };

    float sizeRatio = 48.0f / 64.0f;

    gProtag = new Entity(
        {20.0f, 20.0f},
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
    //initially set all blocks to be of the type "TILE" and initialize their parameters
    for (int i = 0; i<NUMBER_OF_TOTAL_BLOCKS; i++) {
        gTiles[i].setTexture("assets/block.png");
        gTiles[i].setScale({TILE_DIMENSION, TILE_DIMENSION});
        gTiles[i].setColliderDimensions({TILE_DIMENSION, TILE_DIMENSION});
        gTiles[i].setEntityType(TILE);
    }
    //each number corresponds to the height of each block along the terrain
    vector<int> terrain =
        {1, 1, 2, 3, 4, 5, 4, 3, 3, 3, 2, 2, 1, 1, 0, 1, 2, 3, 4, 4, 4, 4, 5, 5, 6, 7, 8,
        9, 10, 10, 11, 12, 13, 14, 15, 14, 13, 12, 12, 11, 11, 10, 9, 9, 9, 8, 7, 6, 5, 4};

    //set the position of each tile based on the terrain
    for (size_t i = 0; i<NUMBER_OF_TOTAL_BLOCKS-3; i++) {
        gTiles[i].setPosition({
            i * TILE_DIMENSION,
            SCREEN_HEIGHT - terrain[i] * TILE_DIMENSION
        });
    }

    //indicates where the diamond ore blocks will be placed as the success
    //platform by setting those tiles to type "PLATFORM"
    vector<int> platformLocations = {7, 14, 19, 46};
    for (size_t i = 0; i<platformLocations.size(); i++) {
        gTiles[platformLocations[i]].setTexture("assets/diamond_ore.png");
        gTiles[platformLocations[i]].setEntityType(PLATFORM);
    }

    //moving platform
    gTiles[NUMBER_OF_TOTAL_BLOCKS-2].setTexture("assets/diamond_ore.png");
    gTiles[NUMBER_OF_TOTAL_BLOCKS-2].setEntityType(MOVINGPLATFORM);
    gTiles[NUMBER_OF_TOTAL_BLOCKS-2].setPosition({SCREEN_WIDTH - 105.0, 350.0f});
    gTiles[NUMBER_OF_TOTAL_BLOCKS-2].setSpeed(50.0);

    gTiles[NUMBER_OF_TOTAL_BLOCKS-1].setEntityType(MOVINGTILE);
    gTiles[NUMBER_OF_TOTAL_BLOCKS-1].setPosition(ORIGIN);
    gTiles[NUMBER_OF_TOTAL_BLOCKS-1].setSpeed(150.0f);

    gTiles[NUMBER_OF_TOTAL_BLOCKS-3].setEntityType(MOVINGTILE);
    gTiles[NUMBER_OF_TOTAL_BLOCKS-3].setPosition({200.0f, 200.0f});
    gTiles[NUMBER_OF_TOTAL_BLOCKS-3].setSpeed(100.0f);
    gTiles[NUMBER_OF_TOTAL_BLOCKS-3].changeDirection(LEFT);

    gExplosionTexture = LoadTexture("assets/explosion.png");
    SetTargetFPS(FPS);
}
void processInput() {
    gProtag->resetAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
    //with fuel, keys W, A, D changes the acceleration of the player in the corresponding directions
    //player should drift in the same direction horizontally until an opposing movement key is pressed
    //pressing 'W' will propel the player upward and letting go will have gravity counteract the upward acceleration
    if (fuel > 0) {
        if (IsKeyDown(KEY_W)) {
            gProtag->thrust(THRUST);
            if (fuel > 0 ) fuel -= fuelConsumption;
        }
        if (IsKeyDown(KEY_A)) {
            gProtag->accelerateLeft(ACCELERATION);
            if (fuel > 0 ) fuel -= fuelConsumption;
        }
        else if (IsKeyDown(KEY_D)) {
            gProtag->accelerateRight(ACCELERATION);
            if (fuel > 0 ) fuel -= fuelConsumption;
        }
    }else { if (fuel < 0) fuel = 0; }

    if (getLength(gProtag->getMovement()) > 1.0f) {
        gProtag->normalizeMovement();
    }

    //resets game
    if (IsKeyPressed(KEY_R)) {
        gProtag->setPosition({20.0f, 20.0f});
        gProtag->resetAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
        gProtag->resetMissionStatus();
        gProtag->resetVelocity();
        fuel = MAX_FUEL;
        kirbyExplode = false;
        missionSuccess = false;
    }
    //closes the game window
    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() {
    float ticks = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    gProtag->update(deltaTime, gTiles, NUMBER_OF_TOTAL_BLOCKS);

    for (int i = 0; i<NUMBER_OF_TOTAL_BLOCKS; i++) {
        gTiles[i].update(deltaTime, nullptr, 0);
    }

    //prevents the player from moving off screen
    if (gProtag->getPosition().x < -30.0f || gProtag->getPosition().y < -30.0f ||
        gProtag->getPosition().x > SCREEN_WIDTH + 30.0 || gProtag->getPosition().y > SCREEN_HEIGHT + 30.0) {
        gAppStatus = TERMINATED;
    }

    //determine the status of the mission when a game-over condition occurs
    if (gProtag->getMissionStatus() == FAIL) { kirbyExplode = true; }
    else if (gProtag->getMissionStatus() == SUCCESS) { missionSuccess = true; }
}

void render() {
    BeginDrawing();
    ClearBackground(BLACK);

    gProtag->render();
    for (int i = 0; i<NUMBER_OF_TOTAL_BLOCKS; i++) {
        gTiles[i].render();
    }
    DrawText(TextFormat("Fuel: %.2f", fuel), 100.0, 50.0, 30.0, WHITE);
    if (kirbyExplode) {
        renderObject(&gExplosionTexture, &gProtag->getPosition(), &gScaleExplosion);
        DrawText("MISSION FAIL", gMissionStatusPosition.x, gMissionStatusPosition.y, 50.0, RED);
    }else if (missionSuccess) {
        DrawText("MISSION SUCCESS", gMissionStatusPosition.x, gMissionStatusPosition.y, 50.0, GREEN);
    }

    EndDrawing();
}

void shutdown() {
    delete gProtag;
    delete[] gTiles;
    UnloadTexture(gExplosionTexture);
    CloseWindow();
}

int main() {
    initialize();
    while (gAppStatus == RUNNING) {
        processInput();
        if (!kirbyExplode && !missionSuccess) {
            update();
        }
        render();
    }

    shutdown();
    return 0;
}