

/* server端提供消息的转发，用户登录，房间控制服务
 * 
 * */

#ifndef __CR_SERVER_HPP__
#define __CR_SERVER_HPP__

#include <string>
#include "include/NetService.hpp"
#include "Worker.hpp"

class CRServer {
public:
    CRServer(std::string ip, short port):mIP(ip), mPort(port),
        mWorker("Server worker"), mNetService(&mWorker, "Server NetService")
    {
    }
    ~CRServer()
    {
        mNetService.Stop();
        mWorker.Stop();
    }

public:
    int Start();

private:
    std::string mIP;
    short mPort;
    Worker mWorker;
    NetService mNetService;
};

#endif


