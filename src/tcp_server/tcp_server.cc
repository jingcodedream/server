/*
 * tcp_server.cc
 *
 *  Created on: 2016年5月26日
 *      Author: joe
 */

#include "src/tcp_server/tcp_server.h"
#include "src/io_server/epoll_io_server.h"
#include "src/session/listen_session.h"

TCPServer::TCPServer() : io_server_(new EpollIOServer()) {
}

int32_t TCPServer::OnListen() {
    SessionInterface *listen_session(new ListenSession("127.0.0.1", 23333, 100, io_server_));
    if (listen_session->Init() != 0) {
        return -1;
    }
    io_server_->AddEvents(IOOptionRead, listen_session->GetFd(), listen_session);
    return 0;
}

void TCPServer::OnConnect() {
    return;
}

int32_t TCPServer::Init() {
    if (OnListen() != 0) {
        return -1;
    }
    OnConnect();
    return 0;
}

void TCPServer::RunForever() {
    io_server_->RunForever();
}


