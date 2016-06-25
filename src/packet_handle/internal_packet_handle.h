/*
 * internal_packet_handle.h
 *
 *  Created on: 2016年6月25日
 *      Author: joe
 */

#ifndef SRC_PACKET_HANDLE_INTERNAL_PACKET_HANDLE_H_
#define SRC_PACKET_HANDLE_INTERNAL_PACKET_HANDLE_H_

#include "src/packet_handle/packet_handle_interface.h"
#include "logger.h"

class InternalPacketHandle : public PacketHandleInterface {
  public:
  private:
    DECL_LOGGER(logger);
};



#endif /* SRC_PACKET_HANDLE_INTERNAL_PACKET_HANDLE_H_ */
