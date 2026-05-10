#pragma once
#include "cpup/canis.h"
#include "cpup/scene.h"
#include "cpup/model.h"
#include "cpup/inputmanager.h"

#include <SDL3/SDL.h>

typedef struct {
    int leftScore;
    int rightScore;
    int lastPaddleHit; // 0 is none, 1 is left, 2 is right
} Ball;

Entity* SpawnBall(AppContext* _app, Entity* _entity);

void BallStart(AppContext* _app, Entity* _entity) {
    _entity->color = InitVector4(1.0f, 1.0f, 1.0f, 1.0f);

    _entity->transform.scale = InitVector3(32.0f, 32.0f, 1.0f);

    if (_entity->data) {
        Ball* ball = (Ball*)_entity->data;
        ball->lastPaddleHit = 0;
    }
}

static bool RectOverlap(Vector3 aPos, Vector3 aScale, Vector3 bPos, Vector3 bScale)
{
    Vector3 aMin = InitVector3(aPos.x - aScale.x * 0.5f, aPos.y - aScale.y * 0.5f, 0.0f);
    Vector3 aMax = InitVector3(aPos.x + aScale.x * 0.5f, aPos.y + aScale.y * 0.5f, 0.0f);
    Vector3 bMin = InitVector3(bPos.x - bScale.x * 0.5f, bPos.y - bScale.y * 0.5f, 0.0f);
    Vector3 bMax = InitVector3(bPos.x + bScale.x * 0.5f, bPos.y + bScale.y * 0.5f, 0.0f);

    return (aMin.x <= bMax.x && aMax.x >= bMin.x) &&
           (aMin.y <= bMax.y && aMax.y >= bMin.y);
}

void BallUpdate(AppContext* _app, Entity* _entity) {

    if (GetKeyDown(_app, SDL_SCANCODE_P))
    {
        SpawnBall(_app, _entity);
    }

    if (Vec2EqualsZero(_entity->velocity) && GetKeyDown(_app, SDL_SCANCODE_SPACE))
    {
        i32 startingDirection = rand() % 4;

        static Vector2 directions[4] = {
            (Vector2){0.72f, 0.72f},
            (Vector2){0.72f, -0.72f},
            (Vector2){-0.72f, 0.72f},
            (Vector2){-0.72f, -0.72f},
        };

        _entity->velocity = Vec2Mul(directions[startingDirection], 150.0f);
    }

    // check if ball is heading below the screen
    if (_entity->transform.position.y - _entity->transform.scale.y * 0.5f <= 0.0f && _entity->velocity.y < 0.0f)
        _entity->velocity.y *= -1.0f; 
    
    // check if ball is heading above the screen
    if (_entity->transform.position.y + _entity->transform.scale.y * 0.5f >= _app->windowHeight && _entity->velocity.y > 0.0f)
        _entity->velocity.y *= -1.0f; 

    Vector3 delta = Vec2ToVec3(Vec2Mul(_entity->velocity, _app->deltaTime));
    _entity->transform.position = Vec3Add(_entity->transform.position, delta);

    // Check for score
    if (_entity->transform.position.x < 0.0f) {
        // Right player scores
        _app->rightScore++;
        // Reset ball
        _entity->transform.position = InitVector3(_app->windowWidth * 0.5f, _app->windowHeight * 0.5f, 0.0f);
        _entity->velocity = InitVector2(0.0f, 0.0f);
        _entity->color = InitVector4(1.0f, 1.0f, 1.0f, 1.0f);
    } else if (_entity->transform.position.x > _app->windowWidth) {
        // Left player scores
        _app->leftScore++;
        // Reset ball
        _entity->transform.position = InitVector3(_app->windowWidth * 0.5f, _app->windowHeight * 0.5f, 0.0f);
        _entity->velocity = InitVector2(0.0f, 0.0f);
        _entity->color = InitVector4(1.0f, 1.0f, 1.0f, 1.0f);
    }

    Ball* ball = (Ball*)_entity->data;
    int currentPaddleHit = 0;

    Entity* lp = Find(&_app->scene, "leftPaddle");
    if (lp && RectOverlap(_entity->transform.position, _entity->transform.scale, lp->transform.position, lp->transform.scale)) {
        currentPaddleHit = 1;
    }

    Entity* rp = Find(&_app->scene, "rightPaddle");
    if (rp && RectOverlap(_entity->transform.position, _entity->transform.scale, rp->transform.position, rp->transform.scale)) {
        currentPaddleHit = 2;
    }

    if (ball && currentPaddleHit != 0 && ball->lastPaddleHit != currentPaddleHit) {
        if (currentPaddleHit == 1) {
            _entity->velocity.x = fabsf(_entity->velocity.x);
            _entity->color = lp->color;
            _entity->transform.position.x = lp->transform.position.x + (lp->transform.scale.x + _entity->transform.scale.x) * 0.5f + 1.0f;
        } else {
            _entity->velocity.x = -fabsf(_entity->velocity.x);
            _entity->color = rp->color;
            _entity->transform.position.x = rp->transform.position.x - (rp->transform.scale.x + _entity->transform.scale.x) * 0.5f - 1.0f;
        }

        ball->lastPaddleHit = currentPaddleHit;
    }

    if (ball && currentPaddleHit == 0) {
        ball->lastPaddleHit = 0;
    }
}

void BallDraw(AppContext* _app, Entity* _entity) {
    Matrix4 transform = IdentityMatrix4(); // the order is important
    Mat4Translate(&transform, _entity->transform.position);
    Mat4Rotate(&transform, _entity->transform.rotation * DEG2RAD, InitVector3(0.0f, 0.0f, 1.0f));
    Mat4Scale(&transform, InitVector3(_entity->transform.scale.x, _entity->transform.scale.y, _entity->transform.scale.z));

    BindShader(_entity->shaderId);

    ShaderSetFloat(_entity->shaderId, "TIME", _app->time);
    ShaderSetMatrix4(_entity->shaderId, "VIEW", _app->view);
    ShaderSetMatrix4(_entity->shaderId, "PROJECTION", _app->projection);

    ShaderSetVector4(_entity->shaderId, "COLOR", _entity->color);
    ShaderBindTexture(_entity->shaderId, _entity->image->id, "MAIN_TEXTURE", 0);
    ShaderSetMatrix4(_entity->shaderId, "TRANSFORM", transform);
    DrawModel(*_entity->model);

    UnBindShader();
}

void BallOnDestroy(AppContext* _app, Entity* _entity) {

}

Entity* SpawnBall(AppContext* _app, Entity* _entity) {
    Scene** scene = &(_app->scene);
    Entity* ball = Spawn(scene);
    ball->transform.position = InitVector3(_app->windowWidth * 0.5f, _app->windowHeight * 0.5f, 0.0f);
    ball->data = calloc(1, sizeof(Ball));
    ball->image = _entity->image;
    ball->model = _entity->model;
    ball->shaderId = _entity->shaderId;
    ball->Start = BallStart;
    ball->Update = BallUpdate;
    ball->Draw = BallDraw;
    ball->OnDestroy = BallOnDestroy;
    return ball;
}
