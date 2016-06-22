/*
 * accept_session.cc
 *
 *  Created on: 2016年6月7日
 *      Author: joe
 */

#include <iostream>
#include <sys/socket.h>

#include "src/session/connect_session.h"

int32_t ConnectSession::Init() {
    return 0;
}

IOStatus ConnectSession::OnRead() {
    char temp_buff[1024];
    int32_t ret = recv(fd_, temp_buff, sizeof(temp_buff), 0);
    if(ret < 0) {
        return IOStatusError;
    } else if (ret == 0) {
        return IOStatusSuccess;
    }
    std::cout << temp_buff << std::endl;
    return IOStatusContinue;
}


