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
typedef uint32_t IOOption; //io选项
const IOOption IOOptionEmpty = 0x0000;
const IOOption IOOptionRead  = 0x0001;
const IOOption IOOptionWrite = 0x0002;
const IOOption IOOptionReadWrite  = 0x0003;
const IOOption IOOPtionError = 0x0004;

inline std::string IOOption2String(IOOption io_option) {
    std::string temp_str;
    switch (io_option) {
        case IOOptionEmpty:{
            temp_str = "IOOptionEmpty";
            break;
        }
        case IOOptionRead:{
            temp_str = "IOOptionRead";
            break;
        }
        case IOOptionWrite:{
            temp_str = "IOOptionWrite";
            break;
        }
        case IOOptionReadWrite:{
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

class SessionInterface;

class IOServerInterface {
  public:
    virtual ~IOServerInterface(){}
    virtual bool RunOnce() = 0;
    virtual void RunForever() = 0;
    virtual int32_t Init() = 0;
    virtual IOOption AddEvents(IOOption op, uint32_t fd, SessionInterface *session) = 0;
    virtual IOOption DelEvents(uint32_t events, uint32_t fd) = 0;
    virtual int32_t WaitEvents() = 0;
};

#endif /* SRC_CORE_IOSERVER_H_ */
