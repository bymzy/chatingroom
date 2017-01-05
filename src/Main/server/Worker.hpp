

#ifndef __CR_SERVER_WORKER_HPP__
#define __CR_SERVER_WORKER_HPP__

#include "include/LogicService.hpp"
#include "Struct.hpp"

class Worker : public LogicService {
public:
    Worker(LogicService *net, std::string name): LogicService(name),
        mNet(net)
    {
    }
    virtual ~Worker()
    {
    }

private:
    virtual int Init();
    virtual int Finit();
    virtual bool Process(OperContext *ctx);

private:
    void RecvMessage(OperContext *ctx);
    void HandleLogon(Msg*msg, std::string ip, unsigned short port, uint64_t connId);
    void GetUserList(uint64_t connId, uint32_t roomId, Msg* msg);
    void SendMessage(uint64_t connId, Msg *msg);

public:
    map_id_user mIdUser;
    LogicService *mNet;
};


#endif


