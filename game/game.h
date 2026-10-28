#ifndef LUNARLANDER_GAME_H
#define LUNARLANDER_GAME_H
#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include <iostream>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <map>
#include <vector>
#include <string>

#define LOG(argument) std::cout << argument << '\n'

enum AppStatus   { TERMINATED, RUNNING };
enum TextureType { SINGLE, ATLAS       };
enum WinCondition {};
void normalize(Vector2* vector);
float getLength(const Vector2& vector);
Rectangle getUVRectangle(Texture2D* texture, int index, int rows, int cols);

#endif //LUNARLANDER_GAME_H