/*
 * listen_session.cc
 *
 *  Created on: 2016年6月7日
 *      Author: joe
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>

#include "src/session/listen_session.h"
#include "src/session/connect_session.h"

IMPL_LOGGER(ListenSession, logger);

int32_t ListenSession::Init() {
    LOG_INFO(logger, "Init Listen Session");
    LOG_DEBUG(logger, "Create Listen Socket");
    fd_ = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (fd_ < 0) {
        LOG_ERROR(logger, "Create Listen Socket Failed, errno="<<errno<<", err_str="<<strerror(errno));
        return -1;
    }
    int32_t reuse = 1;
    LOG_DEBUG(logger, "Set Listen Socket Reuse");
    if  (setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0) {
        LOG_ERROR(logger, "Set Listen Socket Failed, errno="<<errno<<", err_str="<<strerror(errno));
        return -2;
    }
    struct sockaddr_in listen_addr;
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = htons(listen_port_);
    listen_addr.sin_addr.s_addr = inet_addr(listen_ipv4_.c_str());
    LOG_DEBUG(logger, "Listen Socket Bind Port, port="<<listen_port_);
    if (bind(fd_, (sockaddr *)&listen_addr, sizeof(sockaddr)) != 0) {
        LOG_ERROR(logger, "Listen Socket Bind Port Failed, errno="<<errno<<", err_str="<<strerror(errno));
        return -3;
    }
    LOG_DEBUG(logger, "Listen Socket Listen");
    if (listen(fd_, listen_max_connect_) != 0) {
        LOG_ERROR(logger, "Listen Socket Listen Failed, errno="<<errno<<", err_str="<<strerror(errno));
        return -4;
    }
    LOG_INFO(logger, "Init Listen Session Success");
    return 0;
}

IOStatus ListenSession::OnRead() {
    LOG_INFO(logger, "Listen Session OnRead Start");
    struct sockaddr_in peer_addr;
    uint32_t peer_addr_len;
    LOG_DEBUG(logger, "Listen Socket Accept, listen_fd="<<fd_);
    int accept_fd = accept(fd_, (sockaddr *)&peer_addr, &peer_addr_len);
    if(accept_fd < 0) {
        LOG_ERROR(logger, "Listen Socket Accept Failed, listen_fd="<<fd_<<", ret="<<accept_fd<<", errno="<<errno<<", err_str="<<strerror(errno));
        return IOStatusError;
    }
    LOG_DEBUG(logger, "Listen Socket Accept, listen_fd="<<fd_);
    SessionInterface *accept_session(new ConnectSession(accept_fd, inet_ntoa(peer_addr.sin_addr), peer_addr.sin_port));
    io_server_->AddEvents(IOOptionRead, accept_fd, accept_session);
    LOG_INFO(logger, "Listen Session OnRead Finished");
    return IOStatusContinue;
}


