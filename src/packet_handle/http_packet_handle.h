/*
 * http_packet_handle.h
 *
 *  Created on: 2016年7月1日
 *      Author: joe
 */

#ifndef SRC_PACKET_HANDLE_HTTP_PACKET_HANDLE_H_
#define SRC_PACKET_HANDLE_HTTP_PACKET_HANDLE_H_

#include "src/packet_handle/packet_handle_interface.h"

class HttpPacketHandle : public PacketHandleInterface {
  public:
    HttpPacketHandle() {}
    ~HttpPacketHandle() {}
    IOStatus ParsePacket(const std::string &req_str);
  private:
    DECL_LOGGER(logger_);
};



#endif /* SRC_PACKET_HANDLE_HTTP_PACKET_HANDLE_H_ */
