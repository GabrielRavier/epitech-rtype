#include "Vec2.hpp"

template <typename T>
constexpr T PI()
{
    return T(M_PI);
}

template <typename T>
constexpr T degToRad(T angle)
{
    return angle * (PI<T>() / T(180));
}

Vec2::Vec2() : x(0), y(0) {}

/* Vec2 Constructor
    Set the vector's x and y components to the parameters supplied */
Vec2::Vec2(float x, float y) : x(x), y(y) {}

/* Add Member Function
    Adds the vector's x, y, and z components with the supplied vector's x and z components */
void Vec2::Add(Vec2 vec)
{
    this->x += vec.x;
    this->y += vec.y;
}

/* Subtract Member Function
    Subtracts the vector's x and y components with the parameters supplied */
void Vec2::Subtract(Vec2 vec)
{
    this->x -= vec.x;
    this->y -= vec.y;
}

/* ScalarMultiplication Member Function
    Multiplies the vector's x and y components with the scalar supplied */
void Vec2::ScalarMultiplication(float scalar)
{
    this->x *= scalar;
    this->y *= scalar;
}

/* Mag Member Function
    Calculates and returns the vector's magnitude */
float Vec2::Mag()
{
    return sqrt(pow(this->x, 2) + pow(this->y, 2));
}

/* Normalize Member Function
    Normalizes the vector */
void Vec2::Normalize()
{
    double mag = this->Mag();

    this->x /= mag;
    this->y /= mag;
}

/* Dot Member Function
    Calculates and returns the scalar result for the dot product of the vector and vector supplied */
float Vec2::Dot(Vec2 vec)
{
    return ((this->x * vec.x) + (this->y * vec.y));
}

/* RotateZ Member Function
    Calculates the rotation for the vector */
void Vec2::RotateZ(float angle)
{
    float theta = degToRad(angle);

    this->x = (this->x * cos(theta)) - (this->y * sin(theta));
    this->y = (this->x * sin(theta)) + (this->y * cos(theta));
}
