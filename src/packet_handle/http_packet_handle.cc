/*
 * http_packet_handle.cc
 *
 *  Created on: 2016年7月1日
 *      Author: joe
 */

#include "src/packet_handle/http_packet_handle.h"

IMPL_LOGGER(HttpPacketHandle, logger_);

IOStatus HttpPacketHandle::ParsePacket(const std::string &req_str) {
    if (req_str.find("Content-Length") == std::string::npos) {
        return IOStatusContinue;
    }
    std::string temp_str = req_str.substr(req_str.find("Content-Length"));
    std::string body_size_str = temp_str.substr(temp_str.find(" "), temp_str.find("\r\n"));
    uint32_t body_size = atoi(body_size_str.c_str());
    if(req_str.find("\r\n\r\n") == std::string::npos) {
        return IOStatusContinue;
    }
    uint32_t head_size = req_str.find("\r\n\r\n");
    std::string head_str = req_str.substr(0, req_str.find("\r\n\r\n"));
    std::string body_str = req_str.substr(req_str.find("\r\n\r\n") + 4);
    LOG_INFO(logger_, "Http Packet Parse Success, head_size="<<head_size
            <<", body_size="<<body_size<<", head_str="<<head_str<<", body_str="<<body_str);
    return IOStatusSuccess;
}

