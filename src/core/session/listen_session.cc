/*
 * listen_session.cc
 *
 *  Created on: 2016年6月7日
 *      Author: joe
 */

#include "session/listen_session.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "connect_session.h"

int32_t ListenSession::Init(){
    int32_t fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if(fd < 0) {
        return -1;
    }
    struct sockaddr_in listen_addr;
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_port = htonl(listen_port_);
    listen_addr.sin_addr.s_addr = inet_addr(listen_ipv4_.c_str());
    if(bind(fd, (sockaddr *)&listen_addr, sizeof(sockaddr)) != 0){
        return -2;
    }
    fd_ = fd;
    std::tr1::shared_ptr<ListenSession> sp_this(this);
    io_server_interface_->AddEvent(IOOptionRead, fd_, sp_this);
    return 0;
}

IOStatus ListenSession::OnRead() {
    struct sockaddr_in peer_addr;
    uint32_t peer_addr_len;
    int accept_fd = accept(fd_, (sockaddr *)&peer_addr, &peer_addr_len);
    if(accept_fd < 0)
    {
        return IOStatusError;
    }
    std::tr1::shared_ptr<ConnectSession> accept_session(new ConnectSession(accept_fd, peer_addr.sin_addr.s_addr, peer_addr.sin_port));
    io_server_interface_->AddEvent(IOOptionRead, accept_fd, accept_session);
    return IOStatusContinue;
}


