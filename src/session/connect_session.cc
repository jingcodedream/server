/*
 * accept_session.cc
 *
 *  Created on: 2016年6月7日
 *      Author: joe
 */

#include "connect_session.h"

int32_t ConnectSession::Init(){
    return 0;
}

IOStatus ConnectSession::OnRead() {
    return IOStatusContinue;
}


