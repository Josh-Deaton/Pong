#pragma once
#include <string.h>
#include "cpup/canis.h"
#include "cpup/scene.h"
#include "cpup/model.h"

typedef struct {
    int var;
} Paddle;

void PaddleStart(AppContext* _app, Entity* _entity) {
    if (_entity->name && strcmp(_entity->name, "leftPaddle") == 0) {
        _entity->color = InitVector4(1.0f, 0.0f, 0.0f, 1.0f); // red
    } else if (_entity->name && strcmp(_entity->name, "rightPaddle") == 0) {
        _entity->color = InitVector4(0.0f, 0.0f, 1.0f, 1.0f); // blue
    } else {
        _entity->color = InitVector4(0.5f, 1.0f, 1.0f, 1.0f);
    }

    _entity->transform.rotation = 0.0f;
    _entity->transform.scale = InitVector3(32.0f, 128.0f, 1.0f);
}

void PaddleUpdate(AppContext* _app, Entity* _entity) {
    float speed = 400.0f;
    float move = 0.0f;

    if (_entity->name && strcmp(_entity->name, "leftPaddle") == 0) {
        if (GetKey(_app, SDL_SCANCODE_W))
            move = speed;
        else if (GetKey(_app, SDL_SCANCODE_S))
            move = -speed;
    } else if (_entity->name && strcmp(_entity->name, "rightPaddle") == 0) {
        if (GetKey(_app, SDL_SCANCODE_UP))
            move = speed;
        else if (GetKey(_app, SDL_SCANCODE_DOWN))
            move = -speed;
    }

    if (move != 0.0f) {
        _entity->transform.position.y += move * _app->deltaTime;

        float halfHeight = _entity->transform.scale.y * 0.5f;
        if (_entity->transform.position.y - halfHeight < 0.0f)
            _entity->transform.position.y = halfHeight;
        else if (_entity->transform.position.y + halfHeight > _app->windowHeight)
            _entity->transform.position.y = _app->windowHeight - halfHeight;
    }
}

void PaddleDraw(AppContext* _app, Entity* _entity) {
    Matrix4 transform = IdentityMatrix4(); // the order is important
    Mat4Translate(&transform, _entity->transform.position);
    Mat4Rotate(&transform, _entity->transform.rotation * DEG2RAD, InitVector3(0.0f, 0.0f, 1.0f));
    Mat4Scale(&transform, InitVector3(_entity->transform.scale.x, _entity->transform.scale.y, _entity->transform.scale.z));

    BindShader(_entity->shaderId);

    ShaderSetFloat(_entity->shaderId, "TIME", _app->time);
    ShaderSetMatrix4(_entity->shaderId, "VIEW", _app->view);
    ShaderSetMatrix4(_entity->shaderId, "PROJECTION", _app->projection);

    ShaderSetVector4(_entity->shaderId, "COLOR", _entity->color);

    Vector2 gridOffset = InitVector2(_app->time * 30.0f, _app->time * 30.0f);
    ShaderSetVector2(_entity->shaderId, "OFFSET", gridOffset);

    ShaderBindTexture(_entity->shaderId, _entity->image->id, "MAIN_TEXTURE", 0);
    ShaderSetMatrix4(_entity->shaderId, "TRANSFORM", transform);
    DrawModel(*_entity->model);

    UnBindShader();
}

void PaddleOnDestroy(AppContext* _app, Entity* _entity) {

}