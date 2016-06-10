/*
 * epoll_io_server.cc
 *
 *  Created on: 2016年6月7日
 *      Author: joe
 */

#include "io_server/epoll_io_server.h"

#include <sys/epoll.h>

static const uint32_t sc_maxevents=1024;
static const uint32_t sc_timeout=3000;

IOServerEpoll::IOServerEpoll():IOServerEpoll(0, sc_maxevents, sc_timeout) { //委托构造函数，c++11支持
}

IOServerEpoll::IOServerEpoll(uint32_t flags, uint32_t maxevents, uint32_t timeout) {
    epfd = epoll_create1(flags);    //建议使用epoll_create1代替epoll_create,最新的实现中epoll已忽略size
    events = (epoll_event*)malloc(sizeof(struct epoll_event) * maxevents);
    this->maxevents = maxevents;
    this->timeout = timeout;
    sleep(30);
}

IOServerEpoll::~IOServerEpoll() {
    close(epfd);
    free(events);
}

IOOption IOServerEpoll::AddEvent(IOOption op, uint32_t fd, std::tr1::shared_ptr<SessionInterface> session) {
    struct epoll_event event;
    if(op & IOOptionRead)
    {
        event.events |= (EPOLLIN | EPOLLET);
    }
    if(op & IOOptionWrite)
    {
        event.events |= EPOLLOUT;
    }
    event.data.fd = fd;
    event.data.ptr = (void*)(session.get());
    return epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
}

IOOption IOServerEpoll::DelEvent(IOOption op, uint32_t fd) {
    struct epoll_event event;
    if(op & IOOptionRead)
    {
        event.events |= (EPOLLIN | EPOLLET);
    }
    if(op & IOOptionWrite)
    {
        event.events |= EPOLLOUT;
    }
    event.data.fd = fd;
    return epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &event);
}

IOOption IOServerEpoll::WaitEvent() {
    return epoll_wait(epfd, events, maxevents, timeout);
}

void IOServerEpoll::RunForever() {
    while(true)
    {
        if(!RunOnce())
        {
            break;
        }
    }
}

bool IOServerEpoll::RunOnce() {
    int ret = WaitEvent();
    if(ret < 0)
    {
        return false;
    }
    for(uint32_t i = 0; i < ret; ++i)
    {
        int fd = events[i].data.fd;
        std::tr1::shared_ptr<SessionInterface> session(events[i].data.ptr);
        uint32_t del_events = 0;
        if((events[i].events & EPOLLIN) == 1)
        {
            IOStatus io_status = session->OnRead();
            if(io_status == IOStatusSuccess)
            {
                del_events |= IOOptionRead;
            }
        }

        if((events[i].events & EPOLLOUT) == 1)
        {
            IOStatus io_status = session->OnWrite();
            if(io_status == IOStatusSuccess)
            {
                del_events |= IOOptionWrite;
            }
        }

        if(del_events != IOOptionEmpty)
        {

        }
    }
    return true;
}



