/*
 * epoll_io_server.h
 *
 *  Created on: 2016年6月7日
 *      Author: joe
 */

#ifndef SRC_CORE_IO_SERVER_EPOLL_IO_SERVER_H_
#define SRC_CORE_IO_SERVER_EPOLL_IO_SERVER_H_

#include "src/io_server/io_server_interface.h"
#include "src/timer/timer_interface.h"
#include "logger.h"

std::string IOEvents2String(IOEvents io_events);

class EpollIOServer : public IOServerInterface {
  public:
    EpollIOServer(std::shared_ptr<TimerInterface> timer);
    EpollIOServer(int32_t flags, uint32_t maxevents, uint32_t timeout, std::shared_ptr<TimerInterface> timer);
    ~EpollIOServer();
    bool RunOnce();
    void RunForever();
    int32_t Init();
    IOEvents AddEvents(IOEvents io_events, SessionInterface *session);
    IOEvents DelEvents(IOEvents io_events, SessionInterface *session);
    int32_t WaitEvents();
  private:
    int epfd_;
    struct epoll_event *events_;
    int32_t flags_;
    uint32_t maxevents_;
    uint32_t timeout_;
    IOEvents io_events_;
    std::shared_ptr<TimerInterface> timer_;
    DECL_LOGGER(logger_);
};

#endif /* SRC_CORE_IO_SERVER_EPOLL_IO_SERVER_H_ */
