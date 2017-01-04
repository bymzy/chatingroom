

#ifndef __CR_CLIENT_HPP__
#define __CR_CLIENT_HPP__

#include <string.h>
#include "include/NetService.hpp"

class CRClient {
public:
    CRClient(std::string ip, short port, std::string user):mIP(ip),
        mPort(port), mUser(user), mNetService(NULL, "")
    {
    }

    ~CRClient()
    {
    }

public:
    int Start();

private:
    std::string mIP;
    short mPort;
    std::string mUser;
    NetService mNetService; 
};


#endif


