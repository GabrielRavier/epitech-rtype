#pragma once

struct Weapon
{
    enum Type
    {
        MISSILETHROWER
    };

    enum Team
    {
        PLAYERS,
        ENEMY,
    };

    int          rateOfFire;
    int          shootTimer;
    int          damage;
    Weapon::Type type;
    Weapon::Team team;
    bool         haveShot;
};
