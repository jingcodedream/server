/*
 * time_interface.h
 *
 *  Created on: 2016年6月27日
 *      Author: joe
 */

#ifndef SRC_TIMER_TIMER_INTERFACE_H_
#define SRC_TIMER_TIMER_INTERFACE_H_

#include <stdint.h>
#include <vector>

class TimerInterface {
  public:
    struct TimerNode {
      public:
        TimerNode() : next_(nullptr), pre_(nullptr), self_ms_(0), user_date_(nullptr) {}
      public:
        struct TimerNode *next_;
        struct TimerNode *pre_;
        uint64_t self_ms_;
        void * user_date_;
    };
    virtual ~TimerInterface() {};
    virtual int32_t Init() = 0;
    virtual int32_t Add(TimerNode *timer_node) = 0;
    virtual void Del(TimerNode *timer_node) = 0;
    virtual int32_t CheckTimeout(uint64_t now_ms, std::vector<TimerNode*> &timer_node_vec) = 0;
    virtual int32_t CheckTimeout(std::vector<TimerNode*> &timer_node_vec) = 0;
};



#endif /* SRC_TIMER_TIMER_INTERFACE_H_ */
