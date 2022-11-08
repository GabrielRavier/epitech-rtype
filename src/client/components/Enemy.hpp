#pragma once

#include <string>
#include <vector>

struct Enemy
{
    enum EnemyType
    {
        BLOP,
        CROP
    };

    Enemy::EnemyType type;
    int              life;
};
