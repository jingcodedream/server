/*
 * epoll_io_server.cc
 *
 *  Created on: 2016年6月7日
 *      Author: joe
 */

#include "src/io_server/epoll_io_server.h"
#include "src/session/session_interface.h"
#include "src/timer/timer_interface.h"

#include <sys/epoll.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <vector>

static const uint32_t sc_maxevents=1024;
static const uint32_t sc_timeout=3000;

std::string IOEvents2String(IOEvents io_events) {
    std::string temp_str;
    switch (io_events) {
        case IOEventsEmpty:{
            temp_str = "IOOptionEmpty";
            break;
        }
        case IOEventsRead:{
            temp_str = "IOOptionRead";
            break;
        }
        case IOEventsWrite:{
            temp_str = "IOOptionWrite";
            break;
        }
        case IOEventsReadWrite:{
            temp_str = "IOOptionReadWrite";
            break;
        }
        default:{
            temp_str = "IOOPtionError";
            break;
        }
    }
    return temp_str;
}

IMPL_LOGGER(EpollIOServer, logger_);

EpollIOServer::EpollIOServer(std::shared_ptr<TimerInterface> timer) : EpollIOServer(0, sc_maxevents, sc_timeout, timer) {} //委托构造函数，c++11支持


EpollIOServer::EpollIOServer(int32_t flags, uint32_t maxevents, uint32_t timeout, std::shared_ptr<TimerInterface> timer)
    : flags_(flags), maxevents_(maxevents), timeout_(timeout), epfd_(-1), events_(NULL)
        , io_events_(IOEventsEmpty), timer_(timer) {}

EpollIOServer::~EpollIOServer() {
    close(epfd_);
    free(events_);
}

int32_t EpollIOServer::Init() {
    LOG_INFO(logger_, "EpollIOServer Init Start");
    if ((epfd_ = epoll_create1(flags_)) < 0) {
        LOG_ERROR(logger_, "Epoll Create Failed, errno="<<errno<<", err_str="<<strerror(errno));
        return -1;
    }
    if ((events_ = (epoll_event*)malloc(sizeof(struct epoll_event) * maxevents_)) == NULL) {
        LOG_ERROR(logger_, "Malloc Events Failed, errno="<<errno<<", err_str="<<strerror(errno));
        return -2;
    }
    LOG_DEBUG(logger_, "sizeof(events_)="<<sizeof(struct epoll_event) * maxevents_);
    memset(events_, 0, (sizeof(struct epoll_event) * maxevents_));
    LOG_INFO(logger_, "EpollIOServer Init Success");
    return 0;
}

IOEvents EpollIOServer::AddEvents(IOEvents io_events, SessionInterface *session) {
    int32_t fd = session->GetFd();
    IOEvents session_io_events = session->GetIOEvents();
    LOG_INFO(logger_, "AddEvents Start, fd="<<fd<<", op is "<<IOEvents2String(io_events));
    struct epoll_event event;
    memset(&event, 0, sizeof(epoll_event));
    if (io_events & IOEventsRead) {
        event.events |= EPOLLIN ;
    }
    if (io_events & IOEventsWrite) {
        event.events |= EPOLLOUT;
    }
    event.data.ptr = session;
    LOG_DEBUG(logger_, "Add Events, fd="<<fd<<", op is "<<IOEvents2String(io_events));
    if (session_io_events == IOEventsEmpty) {
        if (epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &event) != 0) {
            LOG_ERROR(logger_, "Mod Events Failed, fd="<<fd<<", op is "<<IOEvents2String(io_events));
            return IOEventsEmpty;
        }
    } else {
        if (epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &event) != 0) {
            LOG_ERROR(logger_, "Mod Events Failed, fd="<<fd<<", op is "<<IOEvents2String(io_events));
            return IOEventsEmpty;
        }
    }
    session->SetIOEvents(io_events);
    LOG_INFO(logger_, "AddEvents Success");
    return io_events;
}

IOEvents EpollIOServer::DelEvents(IOEvents io_events, SessionInterface *session) {
    int32_t fd = session->GetFd();
    IOEvents session_io_events = session->GetIOEvents();
    LOG_INFO(logger_, "DelEvents Start, fd="<<fd<<", op is "<<IOEvents2String(io_events));
    IOEvents result_io_events = session_io_events & (~io_events);
    if (result_io_events == IOEventsEmpty) {
        if(epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, NULL) != 0) {
            LOG_ERROR(logger_, "Epoll Delete Events failed, fd="<<fd<<", errno="<<errno<<", err_str="<<strerror(errno));
        }
        return IOEventsEmpty;
    } else {
        struct epoll_event event;
        memset(&event, 0, sizeof(epoll_event));
        if (result_io_events & IOEventsRead) {
            event.events |= EPOLLIN;
        }
        if (result_io_events & IOEventsWrite) {
            event.events |= EPOLLOUT;
        }
        event.data.ptr = session;
        if(epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &event) != 0) {
            LOG_ERROR(logger_, "Epoll Delete Events failed, fd="<<fd<<", errno="<<errno<<", err_str="<<strerror(errno));
            return IOEventsEmpty;
        }
        session->SetIOEvents(result_io_events);
    }
    LOG_INFO(logger_, "DelEvents Success, fd="<<fd<<", op is "<<IOEvents2String(io_events));
    return !IOEventsEmpty;
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
    LOG_INFO(logger_, "RunOnce");
    //检查超时session
    std::vector<TimerInterface::TimerNode*> timer_node_vec;
    if (timer_->CheckTimeout(timer_node_vec) != 0) {
        LOG_ERROR(logger_, "Timer Check Timeout Error");
        return false;
    }
    LOG_INFO(logger_, "timer_node_vec size="<<timer_node_vec.size());
    for (std::vector<TimerInterface::TimerNode*>::iterator it = timer_node_vec.begin(); it != timer_node_vec.end(); ++it) {
        SessionInterface *session = static_cast<SessionInterface*>((*it)->user_date_);
        if (session != NULL) {
            int32_t fd = session->GetFd();
            delete session;
            close(fd);
            LOG_DEBUG(logger_, "close fd because timeout, fd="<<fd);
        }
        delete *it;
    }

    int32_t ret = WaitEvents();
    if (ret < 0) {
        LOG_ERROR(logger_, "Wait Events Has Occured Error, errno="<<errno<<", err_str="<<strerror(errno));
        return false;
    }
    for (uint32_t i = 0; i < ret; ++i) {
        SessionInterface *session = static_cast<SessionInterface*>(events_[i].data.ptr);
        int32_t fd = session->GetFd();
        LOG_DEBUG(logger_, "fd Has Prepared, fd="<<fd);
        uint32_t del_events = 0;
        if ((events_[i].events & EPOLLIN) == 1) {
            LOG_DEBUG(logger_, "Read Prepared");
            IOStatus io_status = session->OnRead();
            if (io_status == IOStatusSuccess) {
                del_events |= IOEventsRead;
            } else if (io_status == IOStatusError) {
                del_events |= IOEventsRead | IOEventsWrite;
            }
        }
        if ((events_[i].events & EPOLLOUT) == 1) {
            LOG_DEBUG(logger_, "Write Prepared");
            IOStatus io_status = session->OnWrite();
            if (io_status == IOStatusSuccess) {
                del_events |= IOEventsWrite;
            } else if (io_status == IOStatusError) {
                del_events |= IOEventsRead | IOEventsWrite;
            }
        }
        if(del_events != IOEventsEmpty) {
            if(DelEvents(del_events, session) == IOEventsEmpty) {
                //delete session;
                close(fd);
                LOG_DEBUG(logger_, "close fd io delete, fd="<<fd);
            }
        }
    }
    return true;
}
