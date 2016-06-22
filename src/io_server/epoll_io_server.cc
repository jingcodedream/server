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
#include <iostream>

static const uint32_t sc_maxevents=1024;
static const uint32_t sc_timeout=3000;

EpollIOServer::EpollIOServer() : EpollIOServer(0, sc_maxevents, sc_timeout) {} //委托构造函数，c++11支持


EpollIOServer::EpollIOServer(uint32_t flags, uint32_t maxevents, uint32_t timeout) : maxevents_(maxevents), timeout_(timeout) {
    epfd_ = epoll_create1(flags);    //建议使用epoll_create1代替epoll_create,最新的实现中epoll已忽略size
    events_ = (epoll_event*)malloc(sizeof(struct epoll_event) * maxevents);
}

EpollIOServer::~EpollIOServer() {
    close(epfd_);
    free(events_);
}

IOOption EpollIOServer::AddEvents(IOOption op, uint32_t fd, SessionInterface *session) {
    struct epoll_event event;
    if (op & IOOptionRead) {
        event.events |= EPOLLIN ;
    }
    if (op & IOOptionWrite) {
        event.events |= EPOLLOUT;
    }
    event.data.fd = fd;
    event.data.ptr = (void*)(session);
    int32_t ret = epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &event);
    if (ret != 0) {
        if (errno == EEXIST) {
            ret = epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &event);
            if (ret != 0) {
                return IOOptionEmpty;
            }
        } else {
            return IOOptionEmpty;
        }
    }
    return event.events;
}

IOOption EpollIOServer::DelEvents(IOOption op, uint32_t fd) {
    struct epoll_event event;
    if (op & IOOptionRead) {
        event.events |= EPOLLIN ;
    }
    if (op & IOOptionWrite) {
        event.events |= EPOLLOUT;
    }
    event.data.fd = fd;
    if(epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, &event) != 0) {
        std::cout << "epoll delete evenst error" << std::endl;
        return IOOptionEmpty;
    }
    if(epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, NULL) == ENOENT) {  //检测是否还在epfd检测中
        std::cout << "not exist" << std::endl;
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
    std::cout << "Run Once" << std::endl;
    int32_t ret = WaitEvents();
    if (ret < 0) {
        return false;
    }
    for (uint32_t i = 0; i < ret; ++i) {
        int fd = events_[i].data.fd;
        SessionInterface *session = static_cast<SessionInterface *>(events_[i].data.ptr);
        uint32_t del_events = 0;
        if ((events_[i].events & EPOLLIN) == 1) {
            IOStatus io_status = session->OnRead();
            if (io_status == IOStatusSuccess) {
                del_events |= IOOptionRead;
            } else if (io_status == IOStatusError) {
                del_events |= IOOptionRead | IOOptionWrite;
            }
        }
        if ((events_[i].events & EPOLLOUT) == 1) {
            IOStatus io_status = session->OnWrite();
            if (io_status == IOStatusSuccess) {
                del_events |= IOOptionWrite;
            } else if (io_status == IOStatusError) {
                del_events |= IOOptionRead | IOOptionWrite;
            }
        }
        if(del_events != IOOptionEmpty) {
            if(DelEvents(del_events, fd) == IOOptionEmpty) {
                std::cout << "hahaha" << std::endl;
                delete session;
                close(fd);
            }
        }
    }
    return true;
}
