/*
 * time_interface.h
 *
 *  Created on: 2016年6月27日
 *      Author: joe
 */

#ifndef SRC_TIMER_TIMER_INTERFACE_H_
#define SRC_TIMER_TIMER_INTERFACE_H_

class TimerInterface {
  public:
    struct TimerNode {
        TimerNode *next_ : 0;
        TimerNode *pre_ : 0;
        uint64_t ms_ : 0;
        void * user_date_ : 0;
    };
    virtual ~TimerInterface() {};
    virtual void Init() = 0;
    virtual void Start() = 0;
    virtual void Add(const TimerNode &timer_node) = 0;
    virtual void Del() = 0;
    virtual void CheckTimeout() = 0;
};



#endif /* SRC_TIMER_TIMER_INTERFACE_H_ */
