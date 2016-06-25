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
    int32_t ret = listen_session->Init();
    if (ret != 0) {
        LOG_ERROR(logger, "Init Listen Session Error, ret="<<ret);
        return -1;
    }
    IOEvents ret_io_events = io_server_->AddEvents(IOEventsRead, listen_session);
    if (ret_io_events == IOEventsEmpty) {
        LOG_ERROR(logger, "Listen Session Add Events Error, ret_io_option is "<<IOEvents2String(ret_io_events));
        return -2;
    }
    LOG_INFO(logger, "OnListen Success");
    return 0;
}

int32_t TCPServer::OnConnect() {
    LOG_INFO(logger, "OnConnect Start");
    LOG_INFO(logger, "OnConnect Success");
    return 0;
}

int32_t TCPServer::Init() {
    LOG_INFO(logger, "TCPServer Init Start");
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
    LOG_INFO(logger, "TCPServer Init Success");
    return 0;
}

void TCPServer::RunForever() {
    LOG_INFO(logger, "TCPServer Run Start");
    io_server_->RunForever();
}


