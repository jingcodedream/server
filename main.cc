/*
 * main.cc
 *
 *  Created on: 2016年6月14日
 *      Author: joe
 */

#include <iostream>

#include <src/tcp_server/tcp_server.h>

int main() {
    TCPServer tcp_server = TCPServer();
    if (tcp_server.Init() != 0) {
        std::cout << "tcp_server init error" << std::endl;
        return -1;
    }
    tcp_server.RunForever();
    return 0;
}

