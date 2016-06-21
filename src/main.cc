/*
 * main.cc
 *
 *  Created on: 2016年6月14日
 *      Author: joe
 */

#include <tcp_server/tcp_server.h>

int main() {
    TCPServer tcp_server = TCPServer();
    if(tcp_server.Init() != 0) {
        return -1;
    }
    return 0;
}

