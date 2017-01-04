

#include "Worker.hpp"
#include "MsgType.hpp"

bool
Worker::Process(OperContext*ctx)
{
    bool processed = true;
    debug_log("Worker::Process! ctx type: " << ctx->GetType());

    switch (ctx->GetType())
    {
        case OperContext::OP_RECV:
            RecvMessage(ctx);
            break;
    }
}

int 
Worker::Init()
{
    return 0;
}

int
Worker::Finit()
{
}

void
Worker::RecvMessage(OperContext *ctx)
{
    std::string ip = ctx->mDest.ip;
    unsigned short port = ctx->mDest.port;
    uint64_t connId = ctx->mConnID;

    Msg *msg = ctx->GetMessage();
    int msgType; 
    (*msg) >> msgType;

    trace_log("Worker RecvMessage!, type: " << msgType);
    switch (msgType) 
    {
        case MsgType::c2s_logon:
            HandleLogon(msg, ip, port, connId);
            break;
    }

    delete msg;
    ctx->SetMessage(NULL);

    return;
}

void
Worker::HandleLogon(Msg *msg, std::string ip, unsigned short port, uint64_t connId)
{
    std::string name;
    (*msg) >> name;

    do {
        iter_id_user iter = mIdUser.find(connId);
        if (iter != mIdUser.end()) {
            warn_log("user " << name << " already logond!");
            break;
        }

        User *user = new User(name, ip, port, connId);
        user->SetRoomId(0);
        mIdUser.insert(std::make_pair(connId, user));
        debug_log("HandleUseLogon, user: " << name
                << ", ip: " << ip
                << ", port: " << port
                << ", connid: " << connId);

        SendUserList(connId, user->mRoomId);
    } while(0);

    return;
}

void
Worker::SendUserList(uint64_t connId, uint32_t roomId)
{

}





