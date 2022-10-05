#pragma once

struct Weapon
{
    enum Type
    {
        MISSILETHROWER
    };

    enum Team
    {
        PLAYERS = 0,
        ENEMY   = 1,
    };

    int          rateOfFire;
    int          shootTimer;
    Weapon::Type type;
    Weapon::Team team;
    bool         haveShot;
};
