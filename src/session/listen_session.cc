/*
 * listen_session.cc
 *
 *  Created on: 2016年6月7日
 *      Author: joe
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "src/session/listen_session.h"
#include "src/session/connect_session.h"

int32_t ListenSession::Init() {
    fd_ = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (fd_ < 0) {
        return -1;
    }
    int32_t reuse = 1;
    if  (setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0) {
        return -4;
    }
    struct sockaddr_in listen_addr;
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = htons(listen_port_);
    listen_addr.sin_addr.s_addr = inet_addr(listen_ipv4_.c_str());
    if (bind(fd_, (sockaddr *)&listen_addr, sizeof(sockaddr)) != 0) {
        return -2;
    }
    if (listen(fd_, listen_max_connect_) != 0) {
        return -3;
    }
    return 0;
}

IOStatus ListenSession::OnRead() {
    struct sockaddr_in peer_addr;
    uint32_t peer_addr_len;
    int accept_fd = accept(fd_, (sockaddr *)&peer_addr, &peer_addr_len);
    if(accept_fd < 0) {
        return IOStatusError;
    }
    SessionInterface *accept_session(new ConnectSession(accept_fd, inet_ntoa(peer_addr.sin_addr), peer_addr.sin_port));
    io_server_->AddEvents(IOOptionRead, accept_fd, accept_session);
    return IOStatusContinue;
}


