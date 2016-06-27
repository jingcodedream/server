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
    ~TimeWheelTimer();
    void Init();
    void Start();
    void Add(const TimerNode &timer_node);
    void Del();
    void CheckTimeout();
  private:
    static const uint32_t WHEEL_NUM_ = 4;
    static const uint32_t WHEEL_NODE_NUM_ = 64;
    TimerNode timer_nodes_[WHEEL_NUM_][WHEEL_NODE_NUM_];
    DECL_LOGGER(logger);
};



#endif /* SRC_TIMER_TIME_WHEEL_TIMER_H_ */
