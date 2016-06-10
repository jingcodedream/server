/*
 * CoreServer.h
 *
 *  Created on: 2016年5月13日
 *      Author: joe
 */

#ifndef SRC_CORE_CORESERVER_H_
#define SRC_CORE_CORESERVER_H_

class CoreServer
{
public:
    CoreServer(){};
    virtual ~CoreServer(){};

    void RunForever();

    void Init();
    void OnListen();
    void OnConnect();
};

#endif /* SRC_CORE_CORESERVER_H_ */
