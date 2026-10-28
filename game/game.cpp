#include "game.h"

float GetLength(const Vector2 vector){
    return sqrtf(
        pow(vector.x, 2) + pow(vector.y, 2)
    );
}

void Normalize(Vector2 *vector){
    float magnitude = GetLength(*vector);

    vector->x /= magnitude;
    vector->y /= magnitude;
}

Rectangle getUVRectangle(const Texture2D *texture, int index, int rows, int cols){
    float uCoord = static_cast<float>(index % cols) / static_cast<float>(cols);
    uCoord *= texture->width;

    float vCoord = static_cast<float>(index / cols) / static_cast<float>(rows);
    vCoord *= texture->height;

    float sliceWidth  = texture->width  / static_cast<float>(cols);
    float sliceHeight = texture->height / static_cast<float>(rows);

    return {
        uCoord,     // top-left x-coord
        vCoord,     // top-left y-coord
        sliceWidth, // width of slice
        sliceHeight // height of slice
    };
}