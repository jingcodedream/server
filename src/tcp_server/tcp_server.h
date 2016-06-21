/*
 * tcp_server.h
 *
 *  Created on: 2016年5月13日
 *      Author: joe
 */

#ifndef SRC_CORE_TCPSERVER_H_
#define SRC_CORE_TCPSERVER_H_

#include "io_server/io_server_interface.h"

#include <stdint.h>

#include <memory>

class TCPServer {
public:
    TCPServer();
    ~TCPServer(){};
    void RunForever();
    int32_t Init();
private:
    int32_t OnListen();
    void OnConnect();
    std::shared_ptr<IOServerInterface> io_server_;
};

#endif /* SRC_CORE_CORESERVER_H_ */
