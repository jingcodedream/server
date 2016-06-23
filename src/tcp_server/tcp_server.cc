/*
 * tcp_server.cc
 *
 *  Created on: 2016年5月26日
 *      Author: joe
 */

#include "src/tcp_server/tcp_server.h"
#include "src/io_server/epoll_io_server.h"
#include "src/session/listen_session.h"

IMPL_LOGGER(TCPServer, logger);

TCPServer::TCPServer() : io_server_(new EpollIOServer()) {}

int32_t TCPServer::OnListen() {
    LOG_INFO(logger, "OnListen Start");
    std::string ipv4_1 = "127.0.0.1";
    uint16_t port_1 = 23333;
    uint32_t listen_max_connect = 100;
    LOG_DEBUG(logger, "Create Listen Session, ipv4="<<ipv4_1<<", port="<<23333<<", listen_max_connect="<<100);
    SessionInterface *listen_session(new ListenSession("127.0.0.1", 23333, 100, io_server_));
    LOG_DEBUG(logger, "Init Listen Session");
    int32_t ret = listen_session->Init();
    if (ret != 0) {
        LOG_ERROR(logger, "Init Listen Session Error, ret="<<ret);
        return -1;
    }
    LOG_DEBUG(logger, "Listen Session Add Events, io_option is "<<IOOption2String(IOOptionRead)<<", fd="<<listen_session->GetFd());
    IOOption ret_io_option = io_server_->AddEvents(IOOptionRead, listen_session->GetFd(), listen_session);
    if (ret_io_option == IOOptionEmpty) {
        LOG_ERROR(logger, "Listen Session Add Events Error, ret_io_option is "<<IOOption2String(ret_io_option));
        return -2;
    }
    LOG_INFO(logger, "OnListen Success");
    return 0;
}

int32_t TCPServer::OnConnect() {
    return 0;
}

int32_t TCPServer::Init() {
    int32_t ret = io_server_->Init();
    if (ret != 0) {
        LOG_ERROR(logger, "IO Server Init Error, ret="<<ret);
        return -1;
    }
    ret = OnListen();
    if (ret != 0) {
        LOG_ERROR(logger, "OnListen Error, ret="<<ret);
        return -2;
    }
    ret = OnConnect();
    if (ret != 0) {
        LOG_ERROR(logger, "OnConnect Error, ret="<<ret);
        return -3;
    }
    return 0;
}

void TCPServer::RunForever() {
    io_server_->RunForever();
}


