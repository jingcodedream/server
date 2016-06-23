/*
 * main.cc
 *
 *  Created on: 2016年6月14日
 *      Author: joe
 */

#include <iostream>

#include <src/tcp_server/tcp_server.h>

int main() {
    INIT_LOGGER("conf/log4cplus.conf");
    TCPServer tcp_server = TCPServer();
    if (tcp_server.Init() != 0) {
        //LOG_ERROR(logger, "tcp_server init error");
        return -1;
    }
    tcp_server.RunForever();
    return 0;
}

