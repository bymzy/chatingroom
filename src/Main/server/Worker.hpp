

#ifndef __CR_SERVER_WORKER_HPP__
#define __CR_SERVER_WORKER_HPP__

#include "include/LogicService.hpp"
#include "User.hpp"
#include "RoomKeeper.hpp"

class Worker : public LogicService {
public:
    Worker(LogicService *net, std::string name): LogicService(name),
        mNet(net), mRoomKeeper(this)
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
    void HandleDrop(OperContext *ctx);
    void RecvMessage(OperContext *ctx);

    void SendMessage(uint64_t connId, Msg *msg);
    void HandleLogon(Msg*msg, std::string ip, unsigned short port, uint64_t connId);
    void PublishChatMessage(Msg *msg, uint64_t userId);
    void HandleCreateRoom(Msg *msg, uint64_t connId);
    void HandleJoinRoom(Msg *msg, uint64_t connId);
    void HandleWhisperMsg(Msg *msg, uint64_t connId);

public:
    LogicService *mNet;
    RoomKeeper mRoomKeeper;
};


#endif


