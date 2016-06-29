/*
 * time_wheel_timer.cc
 *
 *  Created on: 2016年6月27日
 *      Author: joe
 */

#include "src/timer/time_wheel_timer.h"

#include <sys/time.h>
#include <errno.h>
#include <string.h>

IMPL_LOGGER(TimeWheelTimer, logger_);

TimeWheelTimer::TimeWheelTimer()
    : now_ms_(0), ms_index_(0), sec_index_(0), min_index_(0), hour_index_(0) {}

int32_t TimeWheelTimer::Init() {
    struct timeval tv;
    if (gettimeofday(&tv, NULL) != 0) {
        LOG_ERROR(logger_, "gettimeofday return error, errno="<<errno<<", err_str="<<strerror(errno));
        return -1;
    }
    now_ms_ = tv.tv_sec*1000 + tv.tv_usec;
    hour_index_ = (now_ms_ >> 28) % 64;
    min_index_ = (now_ms_ >> 20) % 64;
    sec_index_ = (now_ms_ >> 12) % 64;
    ms_index_ = (now_ms_ >> 4) % 64;
    return 0;
}

int32_t TimeWheelTimer::Add(TimerNode *timer_node) {
    uint64_t self_ms = timer_node->self_ms_;
    if (self_ms < now_ms_ || (self_ms - now_ms_ > 0xFFFFFFFF)) {
        LOG_ERROR(logger_, "timer node self_ms_ is invilide self_ms_="<<self_ms);
        return -1;
    }
    uint32_t index = 0;
    uint32_t index_type = 0;
    if (((self_ms >> 28)) % 64 != hour_index_) {
        index = ((self_ms >> 28)) % 64;
        index_type = index_type_hour_;
    } else if (((self_ms >> 20)) % 64 != min_index_) {
        index = ((self_ms >> 20)) % 64;
        index_type = index_type_min_;
    } else if (((self_ms >> 12)) % 64 != sec_index_) {
        index = ((self_ms >> 12)) % 64;
        index_type = index_type_sec_;
    } else if (((self_ms >> 4)) % 64 != ms_index_) {
        index = ((self_ms >> 4)) % 64;
        index_type = index_type_ms_;
    } else {
        LOG_ERROR(logger_, "timer node self_ms_ is invilide self_ms_="<<self_ms);
        return -2;
    }
    TimerNode &temp_timer_node = timer_nodes_[index_type][index];
    while (temp_timer_node.next_ != NULL) {
        temp_timer_node = temp_timer_node.next_;
    }
    temp_timer_node.next_ = timer_node;
    timer_node->pre_ = temp_timer_node;
    timer_node->next_ =  temp_timer_node.next_;
    return 0;
}

void TimeWheelTimer::Del(TimerNode *timer_node) {
    timer_node->pre_->next_ = timer_node->next_;
    if (timer_node->next_ != NULL) {
        timer_node->next_->pre_ = timer_node->pre_;
    }
    delete timer_node;
}

int32_t TimeWheelTimer::CheckTimeout(std::vector<TimerNode*> &timer_node_vec) {
    struct timeval tv;
    if (gettimeofday(&tv, NULL) != 0) {
        LOG_ERROR(logger_, "gettimeofday return error, errno="<<errno<<", err_str="<<strerror(errno));
        return -1;
    }
    return CheckTimeout(tv.tv_sec*1000 + tv.tv_usec, timer_node_vec);
}

int32_t TimeWheelTimer::CheckTimeout(uint64_t now_ms, std::vector<TimerNode*> &timer_node_vec) {
    if (now_ms < now_ms_ || (now_ms - now_ms_ > 0xFFFFFFFF)) {
        LOG_ERROR(logger_, "now_ms < now_ms_ OR now_ms is too big, now_ms="<<now_ms<<",now_ms_"<<now_ms_);
        return -1;
    }
    uint64_t interval_ms = now_ms - now_ms_;
    for (uint32_t i = 0; i < interval_ms/16; ++i) {
        if (timer_nodes_[index_type_ms_][ms_index_].next_ != NULL) {
            timer_node_vec.push_back(timer_nodes_[index_type_ms_][ms_index_].next_);
        }
        now_ms_ += 16;
        ms_index_ = (ms_index_ + 1) % 64;
        uint32_t temp_index = 0;
        uint32_t temp_index_type = 0;
        if (ms_index_ == 0) {
            sec_index_ = (sec_index_ + 1) % 64;
            temp_index = sec_index_;
            temp_index_type = index_type_sec_;
            if (sec_index_ == 0) {
                min_index_ = (min_index_ + 1) % 64;
                temp_index = min_index_;
                temp_index_type = index_type_min_;
                if (sec_index_ == 0) {
                    hour_index_ = (hour_index_ + 1) % 64;
                    temp_index = hour_index_;
                    temp_index_type = index_type_hour_;
                }
            }
        }
        if (temp_index_type != 0) {
            TimerNode *temp_timer_node = timer_nodes_[temp_index_type][temp_index].next_;
            while (temp_timer_node != NULL) {
                TimerNode *temp_temp_timer_node = temp_timer_node;
                Add(temp_timer_node);
                temp_timer_node = temp_timer_node->next_;
            }
        }
    }
    //now_ms_ = now_ms;
    return 0;
}

