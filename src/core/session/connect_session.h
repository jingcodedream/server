/*
 * connect_session.h
 *
 *  Created on: 2016年6月7日
 *      Author: joe
 */

#ifndef SRC_CORE_SESSION_CONNECT_SESSION_H_
#define SRC_CORE_SESSION_CONNECT_SESSION_H_

#include <string>

class ConnectSession : public SessionInterface {
public:
    ConnectSession(int fd, const std::string &peer_ipv4, uint32_t peer_port)
        : fd_(fd), peer_ipv4_(peer_ipv4), peer_port_(peer_port) {}
    ~ConnectSession() {}

    int32_t Init();
    IOStatus OnRead();
    IOStatus OnWrite() {return IOStatusError;}
    IOStatus OnError() {return IOStatusError;}
private:
    int32_t fd_;
    std::string peer_ipv4_;
    uint32_t peer_port_;
};



#endif /* SRC_CORE_SESSION_CONNECT_SESSION_H_ */
