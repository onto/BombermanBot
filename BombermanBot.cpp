#include <iostream>
#include <random>
#include <queue>

#include "GameClientBomberman.h"

bool is_block(BombermanBlocks _block)
{
    return
        _block == BombermanBlocks::Wall ||
        _block == BombermanBlocks::WallDestroyable ||
        _block == BombermanBlocks::MeatChopper ||
        _block == BombermanBlocks::BombTimer1 ||
        _block == BombermanBlocks::BombTimer2 ||
        _block == BombermanBlocks::BombTimer3 ||
        _block == BombermanBlocks::BombTimer4 ||
        _block == BombermanBlocks::BombTimer5 ||
        _block == BombermanBlocks::OtherBomberman ||
        _block == BombermanBlocks::OtherBombBomberman;
}

Direction coord_diff_to_direction(Coord from, Coord to)
{
    if (from.x != to.x)
    {
        return from.x < to.x ? Direction::Right : Direction::Left;
    }
    else
    {
        return from.y > to.y ? Direction::Up : Direction::Down;
    }
}

std::deque<Coord> find_free_path(const BombermanMap &map, Coord player, const std::set<Coord> &excluded)
{
    for (size_t i = 5; i > 0; --i)
    {
        auto path = map.find_free_path(player, excluded, i);
        if (!path.empty())
            return path;
    }

    return {};
}

int main()
{
    srand(static_cast<unsigned int>(time(nullptr)));

    Coord bomb_coord;
    std::deque<Coord> path;
    std::set<Coord> excluded_from_path;

    GameClientBomberman *gcb = new GameClientBomberman([&]()
    {
        auto &map = gcb->get_map();

        if (map.block(map.player()) == BombermanBlocks::DeadBomberman)
        {
            gcb->Blank();
            return;
        }

        auto player = map.player();
        std::cout << "player " << player << std::endl;

        excluded_from_path = map.find_excluded_coords(bomb_coord);
        print_cont(excluded_from_path, "excluded");

        if (!bomb(map.block(bomb_coord)) && map.has_bomb_targets(player))
        {
            path = find_free_path(map, player, excluded_from_path);
            print_cont(path, "free path");

            if (path.empty())
            {
                gcb->Act();
            }
            else
            {
                gcb->Act(coord_diff_to_direction(player, path.front()));
            }

            bomb_coord = player;
            return;
        }
        else
        {
            path = map.find_path_to_next_target(player, excluded_from_path);
            print_cont(path, "path");

            if (!path.empty())
            {
                if (path.size() > 5 && all_targets(map.block(path.front())))
                {
                    gcb->Act(coord_diff_to_direction(player, path.front()));
                }
                else
                {
                    gcb->Go(coord_diff_to_direction(player, path.front()));
                }

                return;
            }
        }

        gcb->Act();
        bomb_coord = player;
        return;
    });
    gcb->Run("52.232.32.105:8080", "ru@ru.ru", "");
    //gcb->Run("172.16.16.2:8080", "onto@onto.onto", "");

    getchar();
}
