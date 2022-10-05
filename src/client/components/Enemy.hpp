#pragma once

#include <string>
#include <vector>

struct Enemy
{
    enum EnemyType
    {
        BLOP
    };

    Enemy::EnemyType type;
    int              life;
};
