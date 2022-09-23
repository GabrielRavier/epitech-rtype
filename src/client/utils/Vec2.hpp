#pragma once

#define _USE_MATH_DEFINES

#include <cmath>

class Vec2
{
public:
    float x;
    float y;

    Vec2();
    Vec2(float x, float y);

    void                Add(Vec2 vec);
    void                Subtract(Vec2 vec);
    void                ScalarMultiplication(float s);
    [[nodiscard]] float Mag() const;
    void                Normalize();
    [[nodiscard]] float Dot(Vec2 vec) const;
    void                Lerp(Vec2 b, float t);
    void                RotateZ(float angle);
};
