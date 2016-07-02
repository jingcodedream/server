/*
 * accept_session.cc
 *
 *  Created on: 2016年6月7日
 *      Author: joe
 */

#include "src/session/connect_session.h"

#include <iostream>
#include <sys/socket.h>
#include <string.h>

IMPL_LOGGER(ConnectSession, logger_);

int32_t ConnectSession::Init() {
    return 0;
}

IOStatus ConnectSession::OnRead() {
    char temp_buff[1024] = {0};
    int32_t ret = recv(fd_, temp_buff, 1023, 0);
    if(ret < 0) {
        return IOStatusError;
    } else if (ret == 0) {
        return IOStatusSuccess;
    }
    temp_buff[strlen(temp_buff) + 1] = 0;
    return packet_handle_->ParsePacket(temp_buff);
}


