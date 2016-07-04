/*
 * server_conf.h
 *
 *  Created on: 2016年7月4日
 *      Author: joe
 */

#ifndef SRC_TCP_SERVER_CONF_TCP_SERVER_CONF_H_
#define SRC_TCP_SERVER_CONF_TCP_SERVER_CONF_H_

#include "logger.h"

#include <string>
#include <map>

struct TCPServerConf {
    struct ListenConf {
        ListenConf() : ipv4_(std::string()), port_(0), blocklog_(0) {}
        std::string ipv4_;
        uint16_t port_;
        uint32_t blocklog_;
    };
    struct ConnectConf {
        ConnectConf() : ipv4_(std::string()), port_(0) {}
        std::string ipv4_;
        uint16_t port_;
    };

    TCPServerConf(std::string pathname) : pathname_(pathname) {}
    int32_t Init();
    std::map<std::string, ListenConf> listen_map_;
    std::map<std::string, ConnectConf> connect_map_;
    std::string pathname_;
    DECL_LOGGER(logger_);
};



#endif /* SRC_TCP_SERVER_CONF_TCP_SERVER_CONF_H_ */
