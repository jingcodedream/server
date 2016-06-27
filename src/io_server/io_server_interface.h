/*
 * io_server_interface.h
 *
 *  Created on: 2016年5月11日
 *      Author: joe
 */

#ifndef SRC_CORE_IO_SERVER_INTERFACE_H_
#define SRC_CORE_IO_SERVER_INTERFACE_H_

#include <stdint.h>
#include <memory>
#include <string>

//enum IOOption
//{
//    IOOptionEmpty,
//    IOOptionRead,
//    IOOptionWrite,
//    IOOptionReadWrite,
//    IOOPtionError
//};
typedef uint32_t IOEvents; //io选项
const IOEvents IOEventsEmpty = 0x0000;
const IOEvents IOEventsRead  = 0x0001;
const IOEvents IOEventsWrite = 0x0002;
const IOEvents IOEventsReadWrite  = 0x0003;
const IOEvents IOEventsError = 0x0004;

class SessionInterface;

class IOServerInterface {
  public:
    virtual ~IOServerInterface(){}
    virtual bool RunOnce() = 0;
    virtual void RunForever() = 0;
    virtual int32_t Init() = 0;
    virtual IOEvents AddEvents(IOEvents io_events, SessionInterface *session) = 0;
    virtual IOEvents DelEvents(IOEvents io_events, SessionInterface *session) = 0;
    virtual int32_t WaitEvents() = 0;
};

#endif /* SRC_CORE_IOSERVER_H_ */
