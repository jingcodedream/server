/*
 * packet_handle_interface.h
 *
 *  Created on: 2016年6月25日
 *      Author: joe
 */

#ifndef SRC_PACKET_HANDLE_PACKET_HANDLE_INTERFACE_H_
#define SRC_PACKET_HANDLE_PACKET_HANDLE_INTERFACE_H_

#include "src/session/session_interface.h"

#include <string>

class PacketHandleInterface {
  public:
    virtual ~PacketHandleInterface() {}
    virtual IOStatus ParsePacket(const std::string &req_str) = 0;
};

#endif /* SRC_PACKET_HANDLE_PACKET_HANDLE_INTERFACE_H_ */
