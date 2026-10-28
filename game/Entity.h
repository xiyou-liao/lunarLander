#ifndef LUNARLANDER_ENTITY_H
#define LUNARLANDER_ENTITY_H

#include "game.h"

enum Direction {UP, LEFT, RIGHT, DOWN};
enum EntityStatus {ACTIVE, INACTIVE};
enum EntityType {PLAYER, TILE, PLATFORM, MOVING, NONE};

class Entity {
public:
    static constexpr int DEFAULT_SIZE = 250;
    static constexpr int DEFAULT_SPEED = 200;
    static constexpr int DEFAULT_FRAME_SPEED = 14;
    static constexpr float Y_COLLISION_THRESHOLD = 0.5f;

    Entity();
    Entity(const Vector2& position, const Vector2& scale, const char* textureFilePath, EntityType entityType);
    Entity(
        const Vector2& position,
        const Vector2& scale,
        const char* textureFilePath,
        TextureType textureType,
        const Vector2& spriteSheetDimensions,
        EntityType entityType,
        std::map<Direction, std::vector<int>> animationAtlas
    );
    ~Entity();

    void update(float deltaTime, Entity* collidableEntities, int collisionCheckCount);
    void render();
    void normalizeMovement() {normalize(&mMovement);}

    void activate() {mEntityStatus = ACTIVE;}
    void deactivate() {mEntityStatus = INACTIVE;}

    bool isActive() const {return mEntityStatus == ACTIVE;}

   //void moveUp() {mMovement.y = -1; mDirection = UP}
    void accelerateLeft(float acceleration) {
        mAcceleration.x = -acceleration;
       // mDirection = LEFT;
       // std::cout << "acclerating left "<< mPosition.x<< " " << mPosition.y << std::endl;
    }
    void accelerateRight(float acceleration) {
        mAcceleration.x = acceleration;
        //mDirection = RIGHT;
        //std::cout <<"accelerating right "<< mPosition.x<< " " << mPosition.y <<std::endl;
    }
    // void accelerateUp() {mAcceleration.y -= 1; mDirection = UP;}
    // void accelerateDown() {mAcceleration.y += 1; mDirection = DOWN;}
    void resetAcceleration(Vector2 acc) {
        mAcceleration = acc;
       // if (mVelocity.y < 0.0f) mDirection = UP;
       // else if (mVelocity.y > 0.0f) mDirection = DOWN;
    }

    void thrust(float amountThrust) {
        mAcceleration.y = -amountThrust;
        //mDirection = UP;
    }
    // void falling() {
    //    // if (mVelocity.y < 0.0f) mDirection = UP;
    //    // else if (mVelocity.y > 0.0f) mDirection = DOWN;
    //    // std::cout <<"y velocity when falling " << mVelocity.y <<std::endl;
    // }

    void setDirection() {
        if (mEntityType != PLAYER) return;
        if (mVelocity.x > 0.0f) mDirection = RIGHT;
        else if (mVelocity.x < 0.0f) mDirection = LEFT;

        if (mVelocity.y > mVelocity.x) mDirection = DOWN;
        else if (mVelocity.y < 0.0f) mDirection = UP;
    }
    bool isOnPlatform() const;
    //Getters
    const Vector2& getPosition() const {return mPosition;}
    const Vector2& getVelocity() const {return mVelocity;}
    const Vector2& getScale() const {return mScale;}
    const Vector2& getAcceleration() const {return mAcceleration;}
    const Vector2& getMovement() const {return mMovement;}
    const Vector2& getColliderDimensions() const {return mColliderDimensions;}
    const Vector2& getSpriteSheetDimensions() const {return mSpriteSheetDimensions;}
    const Direction& getDirection() const {return mDirection;}
    const EntityType& getEntityType() const {return mEntityType;}
    const MissionStatus& getMissionStatus() const {return missionStatus;}

    void setEntityType(EntityType entityType) {mEntityType = entityType;}
    void setPosition(Vector2 newPosition)
    { mPosition = newPosition;                 }
    void setMovement(Vector2 newMovement)
    { mMovement = newMovement;                 }
    void setAcceleration(Vector2 newAcceleration)
    { mAcceleration = newAcceleration;         }
    void setAccelerationX(float newAcceleration) {
        mAcceleration.x += newAcceleration;
    }
    void setScale(Vector2 newScale)
    { mScale = newScale;                       }
    void setTexture(const char *textureFilepath)
    {mTexture = LoadTexture(textureFilepath);}
    void setColliderDimensions(Vector2 newDimensions)
    { mColliderDimensions = newDimensions;     }
    void setSpriteSheetDimensions(Vector2 newDimensions)
    { mSpriteSheetDimensions = newDimensions;  }
    void setSpeed(int newSpeed)
    { mSpeed = newSpeed;                       }
    void setFrameSpeed(int newSpeed)
    { mFrameSpeed = newSpeed;                  }
    //void setJumpingPower(float newJumpingPower)
    // { mJumpingPower = newJumpingPower;         }
    void setAngle(float newAngle)
    { mAngle = newAngle;                       }
    void failMission() {
        missionStatus = FAIL;
    }
    void missionSuccess() {
        missionStatus = SUCCESS;
    }
private:
    Vector2 mPosition;
    Vector2 mMovement;
    Vector2 mVelocity;
    Vector2 mAcceleration;

    Vector2 mScale;
    Vector2 mColliderDimensions;

    Texture2D mTexture;
    TextureType mTextureType;
    Vector2 mSpriteSheetDimensions;

    std::map<Direction, std::vector<int>> mAnimationAtlas;
    std::vector<int> mAnimationIndices;
    Direction mDirection = DOWN;
    int mFrameSpeed;

    int mCurrentFrameIndex = 0;
    float mAnimationTime = 0.0f;

    int mSpeed;
    float mAngle;

    bool mIsCollidingTop    = false;
    bool mIsCollidingBottom = false;
    bool mIsCollidingRight  = false;
    bool mIsCollidingLeft   = false;

    EntityStatus mEntityStatus = ACTIVE;
    EntityType mEntityType;
    bool isColliding(Entity* other) const;
    void checkCollisionY(Entity* collidableEntities, int collisionCheckCount);
    void checkCollisionX(Entity* collidableEntities, int collisionCheckCount);
    void resetColliderFlags()
    {
        mIsCollidingTop    = false;
        mIsCollidingBottom = false;
        mIsCollidingRight  = false;
        mIsCollidingLeft   = false;
    }
    //bool isActive() {return mEntityStatus == ACTIVE;}

    void animate(float deltaTime);
    bool isThrusting;
    MissionStatus missionStatus = INPROGRESS;
};

#endif //LUNARLANDER_ENTITY_H