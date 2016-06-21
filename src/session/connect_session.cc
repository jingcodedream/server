/*
 * accept_session.cc
 *
 *  Created on: 2016年6月7日
 *      Author: joe
 */

#include <iostream>
#include <sys/socket.h>

#include "connect_session.h"

int32_t ConnectSession::Init() {
    return 0;
}

IOStatus ConnectSession::OnRead() {
    char temp_buff[1024];
    if(recv(fd_, temp_buff, sizeof(temp_buff), 0) < 0) {
        return IOStatusError;
    }
    std::cout << temp_buff << std::endl;
    return IOStatusSuccess;
}


