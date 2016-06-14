/*
 * tcp_server.cc
 *
 *  Created on: 2016年5月26日
 *      Author: joe
 */

#include "tcp_server/tcp_server.h"
#include "io_server/epoll_io_server.h"
#include "session/listen_session.h"

TCPServer::TCPServer() : io_server_(new EpollIOServer()) {
}

int32_t TCPServer::OnListen() {
    std::shared_ptr<SessionInterface> listen_session1(new ListenSession("127.0.0.1", 8888, io_server_));
    if(listen_session1->Init() != 0)
    {
        return -1;
    }
    return 0;
}

void TCPServer::OnConnect() {
    return;
}

int32_t TCPServer::Init() {
    if(OnListen() != 0)
    {
        return -1;
    }
    OnConnect();
    return 0;
}

void TCPServer::RunForever() {
    io_server_->RunForever();
}


