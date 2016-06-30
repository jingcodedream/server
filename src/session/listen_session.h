/*
 * listen_session.h
 *
 *  Created on: 2016年6月7日
 *      Author: joe
 */

#ifndef SRC_CORE_SESSION_LISTEN_SESSION_H_
#define SRC_CORE_SESSION_LISTEN_SESSION_H_

#include "src/session/session_interface.h"
#include "src/io_server/io_server_interface.h"
#include "src/timer/timer_interface.h"
#include "logger.h"

#include <string>

class ListenSession : public SessionInterface {
  public:
    ListenSession(const std::string &listen_ipv4, uint16_t listen_port,
            uint32_t listen_max_connect, std::shared_ptr<IOServerInterface> io_server_, std::shared_ptr<TimerInterface> timer)
        : fd_(-1), listen_ipv4_(listen_ipv4), listen_port_(listen_port), listen_max_connect_(listen_max_connect),
            io_server_(io_server_), io_events_(IOEventsEmpty), timer_(timer) {}
    ~ListenSession() {}

    int32_t Init();
    IOStatus OnRead();
    int32_t GetFd() const {return fd_;}
    IOEvents GetIOEvents() const {return io_events_;};
    void SetIOEvents(IOEvents io_events) {io_events_ = io_events;};
    IOStatus OnWrite() {return IOStatusError;}
    IOStatus OnError() {return IOStatusError;}
  private:
    int32_t fd_;
    std::string listen_ipv4_;
    uint32_t listen_port_;
    uint32_t listen_max_connect_;
    std::shared_ptr<IOServerInterface> io_server_;
    std::shared_ptr<TimerInterface> timer_;
    IOEvents io_events_;
    DECL_LOGGER(logger_);
};

#endif /* SRC_CORE_SESSION_LISTEN_SESSION_H_ */
