/*
 * session.h
 *
 *  Created on: 2016年6月7日
 *      Author: joe
 */

#ifndef SRC_CORE_SESSION_INTERFACE_H_
#define SRC_CORE_SESSION_INTERFACE_H_

enum IOStatus {    //当前session的IO状态
    IOStatusSuccess,
    IOStatusContinue,
    IOStatusError
};

class SessionInterface {
public:
    virtual ~SessionInterface(){}

    virtual int32_t Init() = 0;
    virtual IOStatus OnRead() = 0;
    virtual IOStatus OnWrite() = 0;
    virtual IOStatus OnError() = 0;
};

#endif /* SRC_CORE_SESSION_INTERFACE_H_ */
