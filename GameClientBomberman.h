#pragma once

#include <string>
#include <thread>
#include <functional>
#include <easywsclient/easywsclient.hpp>
#ifdef _WIN32
#pragma comment( lib, "ws2_32" )
#include <WinSock2.h>
#endif
#include <assert.h>
#include <stdio.h>
#include <string>
#include <memory>
#include <vector>
#include <cmath>
#include <queue>
#include <algorithm>
#include <iostream>
#include <set>
#include <deque>
#include <map>

#include "BombermanBlocks.h"
#include "BombermanMap.h"

enum class Direction
{
    Up = 0,
    Down,
    Left,
    Right
};

class GameClientBomberman
{
    BombermanMap _map;

    easywsclient::WebSocket *web_socket;
    std::function<void()> message_handler;

    bool is_running;
    std::thread *work_thread;
    void update_func(std::string _path);

public:
    GameClientBomberman(std::function<void()> _message_handler);
    ~GameClientBomberman();

    void Run(std::string server, std::string userEmail, std::string userPassword = "");
    void Go(Direction dir)
    {
        switch (dir)
        {
        case Direction::Up:
            return web_socket->send("UP");
        case Direction::Down:
            return web_socket->send("DOWN");
        case Direction::Left:
            return web_socket->send("LEFT");
        case Direction::Right:
            return web_socket->send("RIGHT");
        }
    }
    void Act()
    {
        web_socket->send("ACT");
    }
    void Act(Direction dir)
    {
        switch (dir)
        {
        case Direction::Up:
            return web_socket->send("ACT, UP");
        case Direction::Down:
            return web_socket->send("ACT, DOWN");
        case Direction::Left:
            return web_socket->send("ACT, LEFT");
        case Direction::Right:
            return web_socket->send("ACT, RIGHT");
        }
    }
    void Blank()
    {
        web_socket->send("");
    }

    const BombermanMap &get_map() const
    {
        return _map;
    }
};
