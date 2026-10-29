#include "Entity.h"

Entity::Entity() :
    mPosition{ 0.0f, 0.0f }, mMovement{ 0.0f, 0.0f },
    mVelocity{ 0.0f, 0.0f }, mAcceleration{ 0.0f, 0.0f },
    mScale{ DEFAULT_SIZE, DEFAULT_SIZE },
    mColliderDimensions{ DEFAULT_SIZE, DEFAULT_SIZE },
    mTexture{}, mTextureType{ SINGLE }, mSpriteSheetDimensions{},
    mAnimationAtlas{}, mAnimationIndices{}, mDirection{ RIGHT },
    mFrameSpeed{ 0 }, mSpeed{ DEFAULT_SPEED }, mAngle{ 0.0f } {}

Entity::Entity(const Vector2& position, const Vector2& scale,
                const char* textureFilePath, EntityType entityType) :
    mPosition{ position }, mMovement{ 0.0f, 0.0f },
    mVelocity{ 0.0f, 0.0f }, mAcceleration{ 0.0f, 0.0f },
    mScale{ scale }, mColliderDimensions{ scale },
    mTexture{ LoadTexture(textureFilePath) }, mTextureType{ SINGLE },
    mSpriteSheetDimensions{}, mAnimationAtlas{}, mAnimationIndices{},
    mDirection{ RIGHT }, mFrameSpeed{ 0 }, mSpeed{ DEFAULT_SPEED }, mEntityType(entityType),
    mAngle{ 0.0f } {}

Entity::Entity(
        const Vector2& position,
        const Vector2& scale,
        const char* textureFilePath,
        TextureType textureType,
        const Vector2& spriteSheetDimensions,
        EntityType entityType,
        const std::map<Direction, std::vector<int>>& animationAtlas
):
    mPosition{ position }, mMovement{ 0.0f, 0.0f },
    mVelocity{ 0.0f, 0.0f }, mAcceleration{ 0.0f, 0.0f },
    mScale{ scale }, mColliderDimensions{ scale },
    mTexture{ LoadTexture(textureFilePath) }, mTextureType{ textureType },
    mSpriteSheetDimensions{ spriteSheetDimensions },
    mAnimationAtlas{ animationAtlas },
    mAnimationIndices{ animationAtlas.at(RIGHT) }, mDirection{ RIGHT },
    mFrameSpeed{ DEFAULT_FRAME_SPEED }, mSpeed{ DEFAULT_SPEED },
    mEntityType(entityType),
    mAngle{ 0.0f } {}

Entity::~Entity() { UnloadTexture(mTexture); }

//loops through a list of collidable entities and checks for collisions
//updates game logic based on what is collided with and how
void Entity::checkCollisionY( Entity *collidableEntities, int collisionCheckCount ){
    for (int i = 0; i < collisionCheckCount; i++) {
        Entity *collidableEntity = &collidableEntities[i];
        //if colliding with an entity, unclip the two objects to prevent being stuck
        if (isColliding(collidableEntity)) {
            float yDistance = fabs(mPosition.y - collidableEntity->mPosition.y);
            float yOverlap  = fabs(
                yDistance - (mColliderDimensions.y / 2.0f) -
                (collidableEntity->mColliderDimensions.y / 2.0f)
            );

            if (mVelocity.y > 0) {
                mPosition.y -= yOverlap;
                mVelocity.y = 0;
                mIsCollidingBottom = true;

                //if the player hits a bad block, mission fail
                //else if the player hits a winning platform, mission success
                if (collidableEntity->mEntityType == TILE) {
                    failMission();
                }else if (collidableEntity->mEntityType == PLATFORM || collidableEntity->mEntityType == MOVING) {
                    missionSuccess();
                }
            }
            else if (mVelocity.y < 0) {
                mPosition.y += yOverlap;
                mVelocity.y = 0;
                mIsCollidingTop = true;
            }
        }
    }
}

void Entity::checkCollisionX( Entity *collidableEntities, int collisionCheckCount ) {
    for (int i = 0; i < collisionCheckCount; i++) {
        Entity *collidableEntity = &collidableEntities[i];

        if (isColliding(collidableEntity)) {
            //corrects for false horizontal collisions
            float yDistance = fabs(mPosition.y - collidableEntity->mPosition.y);
            float yOverlap  = fabs(
                yDistance - (mColliderDimensions.y / 2.0f) -
                (collidableEntity->mColliderDimensions.y / 2.0f)
            );
            if (yOverlap < Y_COLLISION_THRESHOLD) continue;

            float xDistance = fabs(mPosition.x - collidableEntity->mPosition.x);
            float xOverlap  = fabs(
                xDistance - (mColliderDimensions.x / 2.0f) -
                (collidableEntity->mColliderDimensions.x / 2.0f)
            );

            if (mVelocity.x > 0) {
                mPosition.x -= xOverlap;
                mVelocity.x = 0;

                mIsCollidingRight = true;
                if (collidableEntity->mEntityType == TILE) { //fail if hit wrong block
                    failMission();
                }
            }
            else if (mVelocity.x < 0) {
                mPosition.x += xOverlap;
                mVelocity.x = 0;

                mIsCollidingLeft = true;
                if (collidableEntity->mEntityType == TILE) {//fail if hit wrong block
                    failMission();
                }
            }
        }
    }
}

//checks if two entities are colliding
bool Entity::isColliding(Entity *other) const {
    if (other == nullptr) {return false;}
    float xDistance = fabs(mPosition.x - other->getPosition().x) -
        ((mColliderDimensions.x + other->getColliderDimensions().x) / 2.0f);
    float yDistance = fabs(mPosition.y - other->getPosition().y) -
        ((mColliderDimensions.y + other->getColliderDimensions().y) / 2.0f);

    return xDistance < 0.0f && yDistance < 0.0f;
}

//updates frame index to get the correct animation frame
void Entity::animate(float deltaTime) {
    mAnimationIndices = mAnimationAtlas.at(mDirection);

    mAnimationTime += deltaTime;
    float secondsPerFrame = 1.0f / mFrameSpeed;

    if (mAnimationTime >= secondsPerFrame) {
        mAnimationTime = 0.0f;

        mCurrentFrameIndex++;
        mCurrentFrameIndex %= mAnimationIndices.size();
    }
}

//shows a green box for each entity that marks the boundary of its collision zone
// void Entity::displayCollider() {
//     // draw the collision box
//     Rectangle colliderBox = {
//         mPosition.x - mColliderDimensions.x / 2.0f,
//         mPosition.y - mColliderDimensions.y / 2.0f,
//         mColliderDimensions.x,
//         mColliderDimensions.y
//     };
//
//     DrawRectangleLines(
//         colliderBox.x,      // Top-left X
//         colliderBox.y,      // Top-left Y
//         colliderBox.width,  // Width
//         colliderBox.height, // Height
//         GREEN               // Color
//     );
// }

void Entity::update( float deltaTime, Entity *collidableEntities, int collisionCheckCount) {
    if (mEntityStatus == INACTIVE) return;

    resetColliderFlags();

    mVelocity.x += mAcceleration.x * deltaTime;
    mVelocity.y += mAcceleration.y * deltaTime;

    mPosition.y += mVelocity.y * deltaTime;

    mPosition.x += mVelocity.x * deltaTime;

    checkCollisionX(collidableEntities, collisionCheckCount);
    checkCollisionY(collidableEntities, collisionCheckCount);

    if (mEntityType == PLAYER) { //only animate the player
        setDirection();
        animate(deltaTime);
    } else if (mEntityType == MOVING) { //makes the platform move left to right on the screen
        if (mDirection == RIGHT && mPosition.x < 1200.0 - 100.0) mPosition.x += mSpeed * deltaTime;
        else if (mDirection == LEFT && mPosition.x > 100.0) mPosition.x -= mSpeed * deltaTime;
        if (mPosition.x > 1200.0 - 100.0) {
            mPosition.x = 1050.0;
            mDirection = LEFT;
        }
        else if (mPosition.x < 100.0) {
            mPosition.x = 150.0;
            mDirection = RIGHT;
        }
    }
}

void Entity::render()
{
    if (mEntityStatus == INACTIVE) return;

    Rectangle textureArea;

    switch (mTextureType)
    {
    case SINGLE:
        // Whole texture (UV coordinates)
        textureArea = {
            // top-left corner
            0.0f, 0.0f,

            // bottom-right corner (of texture)
            static_cast<float>(mTexture.width),
            static_cast<float>(mTexture.height)
        };
        break;
        case ATLAS:
        textureArea = getUVRectangle(
            &mTexture,
            mAnimationIndices[mCurrentFrameIndex],
            mSpriteSheetDimensions.x,
            mSpriteSheetDimensions.y
        );
        break;
    default:
        break;
    }

    // Destination rectangle – centred on mPosition
    Rectangle destinationArea = {
        mPosition.x,
        mPosition.y,
        static_cast<float>(mScale.x),
        static_cast<float>(mScale.y)
    };

    // Origin inside the source texture (centre of the texture)
    Vector2 originOffset = {
        static_cast<float>(mScale.x) / 2.0f,
        static_cast<float>(mScale.y) / 2.0f
    };

    // Render the texture on screen
    DrawTexturePro(
        mTexture,
        textureArea, destinationArea, originOffset,
        mAngle, WHITE
    );

    //displayCollider();
}