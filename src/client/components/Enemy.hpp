#pragma once

#include <string>
#include <vector>

struct Enemy
{
    enum EnemyType
    {
        BLOP,
        CROP,
        BOSS
    };

    Enemy::EnemyType type;
    int              life;
};
