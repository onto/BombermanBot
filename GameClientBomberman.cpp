#include "GameClientBomberman.h"

#include <iostream>
#include <cmath>
#include <codecvt>
#include <locale>

GameClientBomberman::GameClientBomberman(std::function<void()> _message_handler)
{
    message_handler = _message_handler;
    is_running = false;
}

GameClientBomberman::~GameClientBomberman()
{
    is_running = false;
    work_thread->join();
}

void GameClientBomberman::Run(std::string server, std::string userEmail, std::string userPassword)
{
    is_running = true;
    work_thread = new std::thread(&GameClientBomberman::update_func, this,
                                  "ws://" + server + "/codenjoy-contest/ws?user=" + userEmail + (userPassword.empty() ? "" : "&pwd=" + userPassword));
}

void GameClientBomberman::update_func(std::string _path)
{
#ifdef _WIN32
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData))
        throw std::exception("WSAStartup Failed.\n");
#endif

    web_socket = easywsclient::WebSocket::from_url(_path);
    if (web_socket == nullptr)is_running = false;
    while (is_running)
    {
        web_socket->poll(100);
        web_socket->dispatch([&](const std::string & message)
        {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;

            _map.fill(convert.from_bytes(message));

            this->message_handler();
        });
    }
    if (web_socket)web_socket->close();

#ifdef _WIN32
    WSACleanup();
#endif
}
