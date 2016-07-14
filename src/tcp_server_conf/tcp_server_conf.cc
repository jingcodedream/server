/*
 * server_conf.cc
 *
 *  Created on: 2016年7月4日
 *      Author: joe
 */

#include "src/tcp_server_conf/tcp_server_conf.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

IMPL_LOGGER(TCPServerConf, logger_);

int32_t TCPServerConf::Init() {
    int32_t fd = open(pathname_.c_str(), O_RDONLY);
    if (fd < 0) {
        LOG_ERROR(logger_, "");
    }
    return 0;
}
