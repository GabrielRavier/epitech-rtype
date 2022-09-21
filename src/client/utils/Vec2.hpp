#pragma once

#include <cmath>

using namespace std;

class Vec2
{
public:
    float x;
    float y;
    float z;

public:
    Vec2();
    Vec2(float x_, float y_, float z_);

    void Add(Vec2 b);
    void Subtract(Vec2 b);
    void ScalarMultiplication(float s);
    float Mag();
    void Normalize();
    float Dot(Vec2 b);
    void Lerp(Vec2 b, float t);
    void RotateZ(float angle);
};
