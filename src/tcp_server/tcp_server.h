/*
 * tcp_server.h
 *
 *  Created on: 2016年5月13日
 *      Author: joe
 */

#ifndef SRC_CORE_TCPSERVER_H_
#define SRC_CORE_TCPSERVER_H_

#include "src/io_server/io_server_interface.h"
#include "logger.h"

#include <stdint.h>
#include <memory>

class TCPServer {
  public:
    TCPServer();
    ~TCPServer(){};
    int32_t Init();
    void RunForever();
  private:
    int32_t OnListen();
    int32_t OnConnect();
    std::shared_ptr<IOServerInterface> io_server_;
    DECL_LOGGER(logger);
};

#endif /* SRC_CORE_CORESERVER_H_ */
