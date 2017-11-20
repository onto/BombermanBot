#ifndef BOMBERMANMAP_H
#define BOMBERMANMAP_H

#include <iostream>
#include <tuple>
#include <cmath>
#include <map>
#include <deque>
#include <set>
#include <vector>
#include <algorithm>

#include "BombermanBlocks.h"

template <typename Cont>
void print_cont(const Cont &path, const std::string &desc)
{
    std::cout << desc << " ";

    for (auto &coord : path)
    {
        std::cout << coord << " ";
    }

    std::cout << std::endl;
}

struct Coord
{
    Coord(): x(0), y(0) {}
    Coord(std::size_t x, std::size_t y) : x(x), y(y) {}

    friend std::ostream &operator<<(std::ostream &os, const Coord &c)
    {
        return os << "{" << c.x << ", " << c.y << "}";
    }

    friend bool operator<(const Coord &c1, const Coord &c2)
    {
        return std::tie(c1.x, c1.y) < std::tie(c2.x, c2.y);
    }

    friend bool operator!=(const Coord &c1, const Coord &c2)
    {
        return std::tie(c1.x, c1.y) != std::tie(c2.x, c2.y);
    }

    friend bool operator==(const Coord &c1, const Coord &c2)
    {
        return std::tie(c1.x, c1.y) == std::tie(c2.x, c2.y);
    }

    std::size_t x;
    std::size_t y;
};

class BombermanMap
{
public:
    BombermanMap()
    {
    }

    void fill(const std::wstring &map_str)
    {
        _player = Coord();

        std::size_t size = static_cast<std::size_t>(std::sqrt(map_str.size() - 6));
        if (_map.size() != size * size)
        {
            _map.resize(size * size, BombermanBlocks::Unknown);
            _map_size = size;
        }

        std::size_t chr = 6;
        for (std::size_t y = 0; y < size; y++)
        {
            for (std::size_t x = 0; x < size; x++)
            {
                auto &b = block(x, y);

                b = static_cast<BombermanBlocks>(map_str[chr++]);

                if (b == BombermanBlocks::Bomberman
                        || b == BombermanBlocks::BombBomberman
                        || b == BombermanBlocks::DeadBomberman)
                {
                    _player.x = x;
                    _player.y = y;
                }
            }
        }
    }

    BombermanBlocks block(Coord coord) const
    {
        return block(coord.x, coord.y);
    }

    BombermanBlocks block(std::size_t x, std::size_t y) const
    {
        return _map[y * _map_size + x];
    }

    BombermanBlocks &block(std::size_t x, std::size_t y)
    {
        return _map[y * _map_size + x];
    }

    BombermanBlocks &operator()(std::size_t x, std::size_t y)
    {
        return block(x, y);
    }

    BombermanBlocks operator()(std::size_t x, std::size_t y) const
    {
        return block(x, y);
    }

    Coord player() const
    {
        return  _player;
    }

    bool visit(std::deque<Coord> &queue, std::set<Coord> &visited, Coord current, std::size_t depth) const
    {
        visited.insert(current);

        if (queue.size() == depth)
        {
            return true;
        }
        else
        {
            for (auto &coord : check_coords(current))
            {
                if (empty_block(block(coord)) && visited.count(coord) == 0)
                {
                    queue.push_back(coord);

                    if (visit(queue, visited, coord, depth))
                        return true;
                    else
                        queue.pop_back();
                }
            }
        }

        return false;
    }

    std::deque<Coord> find_free_path(Coord start, const std::set<Coord> &excluded, std::size_t depth) const
    {
        std::deque<Coord> queue;
        std::set<Coord> visited = excluded;
        visit(queue, visited, start, depth);
        return queue;
    }

    static std::array<Coord, 4> check_coords(Coord current)
    {
        std::array<Coord, 4> a
        {
            {
                Coord{current.x + 1, current.y},
                Coord{current.x - 1, current.y},
                Coord{current.x, current.y + 1},
                Coord{current.x, current.y - 1}
            }
        };
        std::random_shuffle(a.begin(), a.end());

        return a;
    }

    std::deque<Coord> find_path_to_next_target(Coord start, const std::set<Coord> &excluded) const
    {
        std::deque<Coord> queue;
        std::set<Coord> visited = excluded;
        std::map<Coord, Coord> coord_to_parent;

        queue.push_back(start);
        coord_to_parent[start] = start;

        while (!queue.empty())
        {
            Coord current = queue.front();
            queue.pop_front();

            if (has_bomb_targets(current))
            {
                std::cout << "attack point " << current << std::endl;

                std::deque<Coord> result;
                while (coord_to_parent[current] != current)
                {
                    result.push_front(current);
                    current = coord_to_parent[current];
                }
                return result;
            }

            visited.insert(current);

            for (auto check_coord : check_coords(current))
            {
                if (visited.count(check_coord) != 0)
                    continue;

                if (coord_to_parent.count(check_coord) != 0)
                    continue;

                if (empty_block(block(check_coord)))
                {
                    queue.push_back(check_coord);
                    coord_to_parent[check_coord] = current;
                }
            }
        }

        return {};
    }

    std::set<Coord> find_free_coord(Coord from, size_t depth) const
    {
        std::vector<int> depth_n;
        std::vector<int> depth_p;

        for (int i = 1; i <= static_cast<int>(depth); ++i)
        {
            depth_n.push_back(i);
            depth_p.push_back(-i);
        }

        auto find_x = [this, from](auto && check_d)
        {
            std::set<Coord> result;

            for (auto d : check_d)
            {
                if (!valid(static_cast<int>(from.x) + d, static_cast<int>(from.y)))
                    break;

                Coord check_coord(static_cast<std::size_t>(static_cast<int>(from.x) + d), from.y);

                auto b = block(check_coord);
                if (b == BombermanBlocks::Wall || b == BombermanBlocks::WallDestroyable)
                    break;

                result.insert(check_coord);
            }

            return result;
        };

        auto find_y = [this, from](auto && check_d)
        {
            std::set<Coord> result;

            for (auto d : check_d)
            {
                if (!valid(static_cast<int>(from.x), static_cast<int>(from.y) + d))
                    break;

                Coord check_coord(from.x, static_cast<size_t>(static_cast<int>(from.y) + d));

                auto b = block(check_coord);
                if (b == BombermanBlocks::Wall || b == BombermanBlocks::WallDestroyable)
                    break;

                result.insert(check_coord);
            }

            return result;
        };

        auto x_p = find_x(depth_p);
        auto x_n = find_x(depth_n);
        auto y_p = find_y(depth_p);
        auto y_n = find_y(depth_n);

        std::set<Coord> free_coord;
        free_coord.insert(from);
        free_coord.insert(x_p.begin(), x_p.end());
        free_coord.insert(x_n.begin(), x_n.end());
        free_coord.insert(y_p.begin(), y_p.end());
        free_coord.insert(y_n.begin(), y_n.end());

        return free_coord;
    }

    bool has_bomb_targets(Coord bomb_coord) const
    {
        for (auto c : find_free_coord(bomb_coord, 3))
        {
            if (target(block(c)))
                return true;
        }

        return false;
    }

    bool valid(int x, int y) const
    {
        return x > 0 && x < static_cast<int>(_map_size)
               && y > 0 && y < static_cast<int>(_map_size);
    }

    std::set<Coord> find_excluded_coords(Coord player_bomb_coord) const
    {
        std::set<Coord> result;

        for (std::size_t y = 0; y < _map_size; y++)
        {
            for (std::size_t x = 0; x < _map_size; x++)
            {
                Coord c(x, y);

                if (c == player_bomb_coord)
                    continue;

                auto b = block(c);

                if (b == BombermanBlocks::BombTimer1)
                {
                    auto e = find_free_coord(c, 3);
                    result.insert(e.begin(), e.end());
                }
                else if (b == BombermanBlocks::BombTimer2)
                {
                    auto e = find_free_coord(c, 2);
                    result.insert(e.begin(), e.end());
                }
                else if (b == BombermanBlocks::BombTimer3)
                {
                    auto e = find_free_coord(c, 1);
                    result.insert(e.begin(), e.end());
                }
            }
        }

        return result;
    }

private:
    std::vector<BombermanBlocks> _map;
    std::size_t _map_size;
    Coord _player;
};

#endif // BOMBERMANMAP_H
