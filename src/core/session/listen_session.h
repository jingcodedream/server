/*
 * listen_session.h
 *
 *  Created on: 2016年6月7日
 *      Author: joe
 */

#ifndef SRC_CORE_SESSION_LISTEN_SESSION_H_
#define SRC_CORE_SESSION_LISTEN_SESSION_H_

#include "session/session_interface.h"
#include "io_server/io_server_interface.h"

#include <string>

class ListenSession : public SessionInterface {
public:
    ListenSession(const std::string &listen_ipv4, uint32_t listen_port, IOServerInterface *io_server_interface)
        : fd_(-1), listen_ipv4_(listen_ipv4), listen_port_(listen_port), io_server_interface_(io_server_interface) {}
    ~ListenSession(){}

    int32_t Init();
    IOStatus OnRead();
    IOStatus OnWrite() {return IOStatusError;}
    IOStatus OnError() {return IOStatusError;}
private:
    int32_t fd_;
    std::string listen_ipv4_;
    uint32_t listen_port_;
    IOServerInterface *io_server_interface_;
};

#endif /* SRC_CORE_SESSION_LISTEN_SESSION_H_ */
