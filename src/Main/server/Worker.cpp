

#include "Worker.hpp"
#include "Err.hpp"
#include "MsgType.hpp"

bool
Worker::Process(OperContext*ctx)
{
    bool processed = true;
    trace_log("Worker::Process! ctx type: " << ctx->GetType());

    switch (ctx->GetType())
    {
        case OperContext::OP_RECV:
            RecvMessage(ctx);
            break;
        case OperContext::OP_SEND:
            mNet->Enqueue(ctx);
            break;
        case OperContext::OP_DROP:
            HandleDrop(ctx);
            break;
        default:
            processed = false;
            break;
    }

    return processed;
}

int 
Worker::Init()
{
    return 0;
}

int
Worker::Finit()
{
    return 0;
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

    trace_log("Worker RecvMessage! type: " << msgType);
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
Worker::HandleLogon(Msg *msg, std::string ip, unsigned short port,
        uint64_t connId)
{
    std::string name;
    (*msg) >> name;
    Msg *reply = new Msg;
    int err = 0;
    std::string errstr;

    err = mRoomKeeper.HandleLogon(name, ip, port, connId, errstr);

    (*reply) << MsgType::s2c_logon_res;
    (*reply) << err;
    (*reply) << "success";

    reply->SetLen();
    SendMessage(connId, reply);
    return;
}

void
Worker::SendMessage(uint64_t connId, Msg *msg)
{
    OperContext *ctx = new OperContext(OperContext::OP_SEND);
    ctx->SetMessage(msg);
    ctx->SetConnID(connId);
    Enqueue(ctx);
    OperContext::DecRef(ctx);
    return;
}

void
Worker::HandleDrop(OperContext *ctx)
{
    /* user dropped */
    uint64_t userId= ctx->mConnID;
    mRoomKeeper.HandleDrop(userId);
}





