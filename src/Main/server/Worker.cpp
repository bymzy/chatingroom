

#include "Worker.hpp"
#include "Err.hpp"
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
        case OperContext::OP_SEND:
            mNet->Enqueue(ctx);
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
    User *user = NULL;

    do {
        iter_id_user iter = mIdUser.find(connId);
        if (iter != mIdUser.end()) {
            warn_log("user " << name << " already logoned!");
            err = Err::user_name_alredy_exist;
            errstr = "user with same name already logoned!";
            break;
        }

        user = new User(name, ip, port, connId);
        user->SetRoomId(0);
        mIdUser.insert(std::make_pair(connId, user));
        debug_log("HandleUseLogon, user: " << name
                << ", ip: " << ip
                << ", port: " << port
                << ", connid: " << connId);
    } while(0);

    (*reply) << MsgType::c2s_logon_res;
    (*reply) << err;
    (*reply) << "success";

    if (0 == err) {
        GetUserList(connId, user->mRoomId, reply); 
    }

    reply->SetLen();
    SendMessage(connId, reply);
    return;
}

void
Worker::GetUserList(uint64_t connId, uint32_t roomId, Msg *msg)
{
    iter_id_user iter = mIdUser.begin();
    User *user = NULL;
    uint32_t count = mIdUser.size();

    (*msg) << count;
    debug_log("GetUserList count: " << mIdUser.size());
    for(; iter != mIdUser.end(); ++iter) {
        user = iter->second;
        user->Encode(msg);
        debug_log("user info " << user->DebugString());
    }

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





