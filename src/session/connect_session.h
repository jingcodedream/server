/*
 * connect_session.h
 *
 *  Created on: 2016年6月7日
 *      Author: joe
 */

#ifndef SRC_CORE_SESSION_CONNECT_SESSION_H_
#define SRC_CORE_SESSION_CONNECT_SESSION_H_

#include "src/session/session_interface.h"
#include "src/packet_handle/packet_handle_interface.h"
#include "src/packet_handle/http_packet_handle.h"
#include "logger.h"

#include <string>

class ConnectSession : public SessionInterface {
  public:
    ConnectSession(int fd, const std::string &peer_ipv4, uint16_t peer_port)
        : fd_(fd), peer_ipv4_(peer_ipv4), peer_port_(peer_port), io_events_(IOEventsEmpty)
          , packet_handle_(new HttpPacketHandle()) {}
    ~ConnectSession() {}

    int32_t Init();
    IOStatus OnRead();
    int32_t GetFd() const {return fd_;}
    IOEvents GetIOEvents() const {return io_events_;};
    void SetIOEvents(IOEvents io_events) {io_events_ = io_events;};
    IOStatus OnWrite() {return IOStatusError;}
    IOStatus OnError() {return IOStatusError;}
  private:
    int32_t fd_;
    std::string peer_ipv4_;
    uint32_t peer_port_;
    IOEvents io_events_;
    std::shared_ptr<PacketHandleInterface> packet_handle_;
    DECL_LOGGER(logger_);
};



#endif /* SRC_CORE_SESSION_CONNECT_SESSION_H_ */
