/*
 * http_packet_handle.cc
 *
 *  Created on: 2016年7月1日
 *      Author: joe
 */

#include "src/packet_handle/http_packet_handle.h"

IOStatus HttpPacketHandle::ParsePacket(std::string req_str) {
    if (req_str.find("Content-Length") == std::string::npos) {
        return IOStatusContinue;
    }
    std::string body_size_str = req_str.substr(req_str.find("Content-Length"));
    return IOStatusSuccess;
}

