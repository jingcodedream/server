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
    LOG_INFO(logger_, "TimeWheelTimer Init Start");
    struct timeval tv;
    if (gettimeofday(&tv, NULL) != 0) {
        LOG_ERROR(logger_, "gettimeofday return error, errno="<<errno<<", err_str="<<strerror(errno));
        return -1;
    }
    LOG_INFO(logger_, "哈哈哈，tv_sec="<<tv.tv_sec<<", tv_usec="<<tv.tv_usec);
    now_ms_ = tv.tv_sec*1000 + tv.tv_usec/1000;
    hour_index_ = (now_ms_ >> 22) % WHEEL_NODE_NUM_;
    min_index_ = (now_ms_ >> 16) % WHEEL_NODE_NUM_;
    sec_index_ = (now_ms_ >> 10) % WHEEL_NODE_NUM_;
    ms_index_ = (now_ms_ >> 4) % WHEEL_NODE_NUM_;
    LOG_DEBUG(logger_, "now_ms_="<<now_ms_<<", hour_index_="<<hour_index_<<", min_index_="<<min_index_
            <<", sec_index_"<<sec_index_<<", ms_index_"<<ms_index_);
    LOG_INFO(logger_, "TimeWheelTimer Init Start Success");
    return 0;
}

int32_t TimeWheelTimer::Add(TimerNode *timer_node) {
    LOG_INFO(logger_, "time wheel timer add timer node start");
    uint64_t self_ms = timer_node->self_ms_;
    if (self_ms < now_ms_ || (self_ms - now_ms_ > 0xFFFFFFF)) {
        LOG_ERROR(logger_, "timer node self_ms_ is invilide self_ms_="<<self_ms<<", now_ms_="<<now_ms_);
        return -1;
    }
    uint32_t index = 0;
    uint32_t index_type = 0;
    if (((self_ms >> 22)) % WHEEL_NODE_NUM_ != hour_index_) {
        index = ((self_ms >> 22)) % WHEEL_NODE_NUM_;
        index_type = index_type_hour_;
    } else if (((self_ms >> 16)) % WHEEL_NODE_NUM_ != min_index_) {
        index = ((self_ms >> 16)) % WHEEL_NODE_NUM_;
        index_type = index_type_min_;
    } else if (((self_ms >> 10)) % WHEEL_NODE_NUM_ != sec_index_) {
        index = ((self_ms >> 10)) % WHEEL_NODE_NUM_;
        index_type = index_type_sec_;
    } else if (((self_ms >> 4)) % WHEEL_NODE_NUM_ != ms_index_) {
        index = ((self_ms >> 4)) % WHEEL_NODE_NUM_;
        index_type = index_type_ms_;
    } else {
        LOG_ERROR(logger_, "timer node self_ms_ is invilide self_ms_="<<self_ms);
        return -2;
    }
    LOG_DEBUG(logger_, "now_ms_="<<now_ms_<<", self_ms="<<self_ms<<", hour_index_="<<hour_index_<<", min_index_="<<min_index_
        <<", sec_index_="<<sec_index_<<", ms_index_="<<ms_index_);
    LOG_DEBUG(logger_, "index_type="<<index_type<<", index="<<index);
    TimerNode *temp_timer_node = &timer_nodes_[index_type][index];
    while (temp_timer_node->next_ != NULL) {
        temp_timer_node = temp_timer_node->next_;
    }
    temp_timer_node->next_ = timer_node;
    timer_node->pre_ = temp_timer_node;
    timer_node->next_ = NULL;
    LOG_INFO(logger_, "time wheel timer add timer node finished");
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
    return CheckTimeout(tv.tv_sec*1000 + tv.tv_usec/1000, timer_node_vec);
}

int32_t TimeWheelTimer::CheckTimeout(uint64_t now_ms, std::vector<TimerNode*> &timer_node_vec) {
    LOG_INFO(logger_, "Check Timeout Start, now_ms_="<<now_ms_);
    if (now_ms < now_ms_ || (now_ms - now_ms_ > 0xFFFFFFF)) {
        LOG_ERROR(logger_, "now_ms < now_ms_ OR now_ms is too big, now_ms="<<now_ms<<",now_ms_="<<now_ms_);
        return -1;
    }
    uint64_t interval_ms_count = (now_ms - now_ms_) >> 4;
    for (uint32_t i = 0; i < interval_ms_count; ++i) {
        TimerNode *temp_timer_node = timer_nodes_[index_type_ms_][ms_index_].next_;
        while (temp_timer_node != NULL) {
            timer_node_vec.push_back(temp_timer_node);
            temp_timer_node = temp_timer_node->next_;
        }
        now_ms_ += 16;
        TimerNode *temp_temp_timer_node = nullptr;
        TimerNode *temp_temp_timer_node_next = nullptr;
        ms_index_ = (ms_index_ + 1) % WHEEL_NODE_NUM_;
        if (ms_index_ == 0) {
            sec_index_ = (sec_index_ + 1) % WHEEL_NODE_NUM_;
            if (sec_index_ == 0) {
                min_index_ = (min_index_ + 1) % WHEEL_NODE_NUM_;
                if (min_index_ == 0) {
                    hour_index_ = (hour_index_ + 1) % WHEEL_NODE_NUM_;
                    temp_temp_timer_node = timer_nodes_[index_type_hour_][hour_index_].next_;
                    while (temp_temp_timer_node != NULL) {
                        temp_temp_timer_node_next = temp_temp_timer_node->next_;
                        Add(temp_temp_timer_node);
                        temp_temp_timer_node = temp_temp_timer_node_next;
                    }
                }
                temp_temp_timer_node = timer_nodes_[index_type_min_][min_index_].next_;
                while (temp_temp_timer_node != NULL) {
                    temp_temp_timer_node_next = temp_temp_timer_node->next_;
                    Add(temp_temp_timer_node);
                    temp_temp_timer_node = temp_temp_timer_node_next;
                }
            }
            temp_temp_timer_node = timer_nodes_[index_type_sec_][sec_index_].next_;
            while (temp_temp_timer_node != NULL) {
                temp_temp_timer_node_next = temp_temp_timer_node->next_;
                Add(temp_temp_timer_node);
                temp_temp_timer_node = temp_temp_timer_node_next;
            }
        }
    }
    now_ms_ = now_ms;
    LOG_INFO(logger_, "Check Timeout finished, now_ms_="<<now_ms_<<", hour_index_="<<hour_index_<<", min_index_="<<min_index_
            <<", sec_index_="<<sec_index_<<", ms_index_="<<ms_index_);
    return 0;
}

