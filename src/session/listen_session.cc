/*
 * listen_session.cc
 *
 *  Created on: 2016年6月7日
 *      Author: joe
 */

#include "src/session/listen_session.h"
#include "src/session/connect_session.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>

IMPL_LOGGER(ListenSession, logger_);

int32_t ListenSession::Init() {
    LOG_INFO(logger_, "Init Listen Session");
    fd_ = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (fd_ < 0) {
        LOG_ERROR(logger_, "Create Listen Socket Failed, errno="<<errno<<", err_str="<<strerror(errno));
        return -1;
    }
    int32_t reuse = 1;
    if  (setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0) {
        LOG_ERROR(logger_, "Set Listen Socket Failed, errno="<<errno<<", err_str="<<strerror(errno));
        return -2;
    }
    struct sockaddr_in listen_addr;
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = htons(listen_port_);
    listen_addr.sin_addr.s_addr = inet_addr(listen_ipv4_.c_str());
    if (bind(fd_, (sockaddr *)&listen_addr, sizeof(sockaddr)) != 0) {
        LOG_ERROR(logger_, "Listen Socket Bind Port Failed, errno="<<errno<<", err_str="<<strerror(errno));
        return -3;
    }
    if (listen(fd_, listen_max_connect_) != 0) {
        LOG_ERROR(logger_, "Listen Socket Listen Failed, errno="<<errno<<", err_str="<<strerror(errno));
        return -4;
    }
    LOG_INFO(logger_, "Init Listen Session Success");
    return 0;
}

IOStatus ListenSession::OnRead() {
    LOG_INFO(logger_, "Listen Session OnRead Start");
    struct sockaddr_in peer_addr;
    uint32_t peer_addr_len;
    int accept_fd = accept(fd_, (sockaddr *)&peer_addr, &peer_addr_len);
    if(accept_fd < 0) {
        LOG_ERROR(logger_, "Listen Socket Accept Failed, listen_fd="<<fd_<<", ret="<<accept_fd<<", errno="<<errno<<", err_str="<<strerror(errno));
        return IOStatusError;
    }
    LOG_DEBUG(logger_, "accept_fd="<<accept_fd);
    SessionInterface *accept_session(new ConnectSession(accept_fd, inet_ntoa(peer_addr.sin_addr), peer_addr.sin_port));
    if ((io_server_->AddEvents(IOEventsRead, accept_session) == IOEventsEmpty)) {
        LOG_ERROR(logger_, "Add Connect Events Error, fd="<<fd_<<", ret="<<IOEventsEmpty);
        return IOStatusError;
    }
    TimerInterface::TimerNode *temp_timer_node = new TimerInterface::TimerNode();
    struct timeval tv;
    if (gettimeofday(&tv, NULL) != 0) {
        LOG_ERROR(logger_, "gettimeofday return error, errno="<<errno<<", err_str="<<strerror(errno));
        return IOStatusError;
    }
    uint64_t time_out_ms = 1000;
    temp_timer_node->self_ms_ = tv.tv_sec*1000 + tv.tv_usec/1000 + time_out_ms;
    temp_timer_node->user_date_ = accept_session;
    if ((timer_->Add(temp_timer_node)) != 0) {
        LOG_ERROR(logger_, "Add Timer Node Error");
        delete temp_timer_node;
        return IOStatusError;
    }
    LOG_INFO(logger_, "Listen Session OnRead Finished");
    return IOStatusContinue;
}


