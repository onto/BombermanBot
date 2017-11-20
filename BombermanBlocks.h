#pragma once

#include <stdint.h>

enum class BombermanBlocks : uint16_t
{
    Unknown = 0,

    Bomberman = L'☺',
    BombBomberman = L'☻',
    DeadBomberman = L'Ѡ',

    OtherBomberman = L'♥',
    OtherBombBomberman = L'♠',
    OtherDeadBomberman = L'♣',

    BombTimer5 = L'5',
    BombTimer4 = L'4',
    BombTimer3 = L'3',
    BombTimer2 = L'2',
    BombTimer1 = L'1',
    Boom = L'҉',

    Wall = L'☼',
    WallDestroyable = L'#',
    DestroyedWall = L'H',

    MeatChopper = L'&',
    DeadMeatChopper = L'x',

    Space = L' '
};

inline bool empty_block(BombermanBlocks block)
{
    return block == BombermanBlocks::DeadBomberman
            || block == BombermanBlocks::OtherDeadBomberman
            || block == BombermanBlocks::Boom
            || block == BombermanBlocks::DestroyedWall
            || block == BombermanBlocks::DeadMeatChopper
            || block == BombermanBlocks::Space;
}

inline bool bomberman_bomb_block(BombermanBlocks block)
{
    return block == BombermanBlocks::BombBomberman;
}

inline bool bomb(BombermanBlocks block)
{
    return block == BombermanBlocks::BombBomberman
            || block == BombermanBlocks::BombTimer5
            || block == BombermanBlocks::BombTimer4
            || block == BombermanBlocks::BombTimer3
            || block == BombermanBlocks::BombTimer2
            || block == BombermanBlocks::BombTimer1;
}

inline bool target(BombermanBlocks block)
{
    return block == BombermanBlocks::OtherBomberman;
}

inline bool all_targets(BombermanBlocks block)
{
    return block == BombermanBlocks::OtherBomberman
            || block == BombermanBlocks::MeatChopper
            || block == BombermanBlocks::WallDestroyable;
}
