#pragma once

#define _USE_MATH_DEFINES

#include <cmath>

class Vec2
{
public:
    float x;
    float y;
    float z;

public:
    Vec2();
    Vec2(float x, float y);

    void Add(Vec2 b);
    void Subtract(Vec2 b);
    void ScalarMultiplication(float s);
    float Mag();
    void Normalize();
    float Dot(Vec2 b);
    void Lerp(Vec2 b, float t);
    void RotateZ(float angle);
};
