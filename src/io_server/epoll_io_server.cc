/*
 * epoll_io_server.cc
 *
 *  Created on: 2016年6月7日
 *      Author: joe
 */

#include "src/io_server/epoll_io_server.h"
#include "src/session/session_interface.h"

#include <sys/epoll.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

static const uint32_t sc_maxevents=1024;
static const uint32_t sc_timeout=3000;

IMPL_LOGGER(EpollIOServer, logger);

EpollIOServer::EpollIOServer() : EpollIOServer(0, sc_maxevents, sc_timeout) {} //委托构造函数，c++11支持


EpollIOServer::EpollIOServer(int32_t flags, uint32_t maxevents, uint32_t timeout)
    : flags_(flags), maxevents_(maxevents), timeout_(timeout), epfd_(-1), events_(NULL) {}

EpollIOServer::~EpollIOServer() {
    close(epfd_);
    free(events_);
}

int32_t EpollIOServer::Init() {
    LOG_INFO(logger, "EpollIOServer Init Start");
    if ((epfd_ = epoll_create1(flags_)) < 0) {
        LOG_ERROR(logger, "Epoll Create Failed, errno="<<errno<<", err_str="<<strerror(errno));
        return -1;
    }
    if ((events_ = (epoll_event*)malloc(sizeof(struct epoll_event) * maxevents_)) == NULL) {
        LOG_ERROR(logger, "Malloc Events Failed, errno="<<errno<<", err_str="<<strerror(errno));
        return -2;
    }
    LOG_INFO(logger, "EpollIOServer Init Success");
    return 0;
}

IOOption EpollIOServer::AddEvents(IOOption op, uint32_t fd, SessionInterface *session) {
    LOG_INFO(logger, "AddEvents Start");
    struct epoll_event event;
    if (op & IOOptionRead) {
        event.events |= EPOLLIN ;
    }
    if (op & IOOptionWrite) {
        event.events |= EPOLLOUT;
    }
    event.data.fd = fd;
    event.data.ptr = (void*)(session);
    LOG_DEBUG(logger, "Add Events, fd="<<fd<<", op is "<<IOOption2String(op));
    int32_t ret = epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &event);
    if (ret != 0) {
        LOG_INFO(logger, "fd is Exist, fd="<<fd);
        if (errno == EEXIST) {
            if (epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &event) != 0) {
                LOG_ERROR(logger, "Mod Events Failed, fd="<<fd<<", op is "<<IOOption2String(op));
                return IOOptionEmpty;
            }
        } else {
            LOG_ERROR(logger, "Add Events Failed, fd="<<fd<<", op is "<<IOOption2String(op));
            return IOOptionEmpty;
        }
    }
    LOG_INFO(logger, "AddEvents Success");
    return op;
}

IOOption EpollIOServer::DelEvents(IOOption op, uint32_t fd) {
    struct epoll_event event;
    if (op & IOOptionRead) {
        event.events |= EPOLLIN;
    }
    if (op & IOOptionWrite) {
        event.events |= EPOLLOUT;
    }
    event.data.fd = fd;
    if(epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, &event) != 0) {
        return IOOptionEmpty;
    }
    if(epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, NULL) == ENOENT) {  //检测是否还在epfd检测中
        return IOOptionEmpty;
    }
    return !IOOptionEmpty;
}

int32_t EpollIOServer::WaitEvents() {
    return epoll_wait(epfd_, events_, maxevents_, timeout_);
}

void EpollIOServer::RunForever() {
    while (true) {
        if (!RunOnce()) {
            break;
        }
    }
}

bool EpollIOServer::RunOnce() {
    LOG_INFO(logger, "RunOnce");
    int32_t ret = WaitEvents();
    if (ret < 0) {
        LOG_ERROR(logger, "Wait Events Has Occured Error, errno="<<errno<<", err_str="<<strerror(errno));
        return false;
    }
    for (uint32_t i = 0; i < ret; ++i) {
        int fd = events_[i].data.fd;
        SessionInterface *session = static_cast<SessionInterface *>(events_[i].data.ptr);
        uint32_t del_events = 0;
        if ((events_[i].events & EPOLLIN) == 1) {
            LOG_DEBUG(logger, "Read Prepared");
            IOStatus io_status = session->OnRead();
            if (io_status == IOStatusSuccess) {
                del_events |= IOOptionRead;
            } else if (io_status == IOStatusError) {
                del_events |= IOOptionRead | IOOptionWrite;
            }
        }
        if ((events_[i].events & EPOLLOUT) == 1) {
            LOG_DEBUG(logger, "Write Prepared");
            IOStatus io_status = session->OnWrite();
            if (io_status == IOStatusSuccess) {
                del_events |= IOOptionWrite;
            } else if (io_status == IOStatusError) {
                del_events |= IOOptionRead | IOOptionWrite;
            }
        }
        if(del_events != IOOptionEmpty) {
            if(DelEvents(del_events, fd) == IOOptionEmpty) {
                delete session;
                close(fd);
            }
        }
    }
    return true;
}
