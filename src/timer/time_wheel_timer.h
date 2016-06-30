/*
 * time_wheel_timer.h
 *
 *  Created on: 2016年6月27日
 *      Author: joe
 */

#ifndef SRC_TIMER_TIME_WHEEL_TIMER_H_
#define SRC_TIMER_TIME_WHEEL_TIMER_H_

#include "src/timer/timer_interface.h"
#include "logger.h"

class TimeWheelTimer : public TimerInterface {
  public:
    TimeWheelTimer();
    ~TimeWheelTimer() {};
    int32_t Init();
    int32_t Add(TimerNode *timer_node);
    void Del(TimerNode *timer_node);
    int32_t CheckTimeout(uint64_t now_ms, std::vector<TimerNode*> &timer_node_vec);
    int32_t CheckTimeout(std::vector<TimerNode*> &timer_node_vec);
  private:
    uint64_t now_ms_;
    const static uint32_t WHEEL_NUM_ = 4;
    const static uint32_t WHEEL_NODE_NUM_ = 64;
    const static uint32_t index_type_hour_ = 3;
    const static uint32_t index_type_min_ = 2;
    const static uint32_t index_type_sec_ = 1;
    const static uint32_t index_type_ms_ = 0;
    TimerNode timer_nodes_[WHEEL_NUM_][WHEEL_NODE_NUM_];
    uint32_t ms_index_;
    uint32_t sec_index_;
    uint32_t min_index_;
    uint32_t hour_index_;
    DECL_LOGGER(logger_);
};



#endif /* SRC_TIMER_TIME_WHEEL_TIMER_H_ */
