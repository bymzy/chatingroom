

#include "Client.hpp"
#include "include/Log.hpp"
#include "MsgType.hpp"
#include "include/common.h"

int CRClient::Init()
{
    int err = 0;

    do {
        err = mNetService.Start();
        if (0 != err) {
            error_log("netservice start failed!");
            break;
        }

        err = mNetService.StartConnectRemote(mIP, mPort, mConnId);
        if (0 != err) {
            error_log("client connect remote failed! ip: " << mIP 
                    << ", port: " << mPort);
            break;
        }
    } while(0);

    return err;
}

void
CRClient::RecvMessage(OperContext *ctx)
{
    Msg *msg = ctx->GetMessage();
    int msgType;
    (*msg) >> msgType;

    trace_log("CRClient RecvMessage! type: " << msgType);

    switch(msgType)
    {
        case MsgType::s2c_logon_res:
            HandleLogonRes(msg);
            break;
        case MsgType::s2c_room_info:
            ParseRoomInfo(msg);
            UpdateWindowUserList();
            break;
        case MsgType::s2c_room_chat_msg:
            ReceiveChatMessage(msg);
            break;
        case MsgType::local_warn_msg:
            ReceiveLocalWarnMessage(msg); 
            break;
        case MsgType::s2c_create_room_res:
            HandleCreateRoomRes(msg);
            break;
    }

    delete msg;
    ctx->SetMessage(NULL);
}

bool
CRClient::Process(OperContext *ctx)
{
    bool processed = true;
    debug_log("CRClient::Process! ctx type: " << ctx->GetType());

    switch (ctx->GetType())
    {
        case OperContext::OP_RECV:
            RecvMessage(ctx);
            break;
        case OperContext::OP_SEND:
            mNetService.Enqueue(ctx);
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
CRClient::Finit()
{
    return 0;
}

void
CRClient::HandleLogonRes(Msg *msg)
{
    int err = 0;
    std::string errstr;
    (*msg) >> err;
    (*msg) >> errstr;

    if (0 == err) {
        mUser.Decode(msg);
    }

    debug_log("handle logon " << err << ", errstr: " << errstr);
    if (0 != err) {
        error_log("logon failed, errno: " << err
                << ", errstr: " << errstr);
        /* TODO exit normally */
        exit(err);
        LogonFailed();
    } else {
        debug_log("current user info: " << mUser.DebugString());
    }
}

void 
CRClient::ParseRoomInfo(Msg *msg)
{
    uint32_t count = 0;
    iter_id_user iter = mOnlines.begin();
    /* clear mOnlines*/
    for (; iter != mOnlines.end(); ++iter) {
        delete iter->second;
    }
    mOnlines.clear();

    mCurrentRoom.Decode(msg);
    trace_log("room info: " << mCurrentRoom.DebugString());

    (*msg) >> count;
    debug_log("parse user list count: " << count);
    for (uint32_t i = 0; i < count; ++i) {
        User *user = new User;
        user->Decode(msg);
        debug_log("user info:" << user->DebugString());
        mOnlines.insert(std::make_pair(user->mID, user));
    }
}

void
CRClient::UpdateWindowUserList()
{
    std::vector<std::string> vec;
    std::string title = "<L></B/U/31>NAME  ID   IP   PORT<!31>";
    vec.push_back(title);

    iter_id_user index = mOnlines.begin();
    for (;index != mOnlines.end(); ++index) {
        User *user = index->second;
        std::string temp;
        temp += "<L></B/31>";
        temp += user->GetName() + "  ";
        temp += i2s(user->GetId()) + "  ";
        temp += user->GetIp() + "  ";
        temp += i2s(user->GetPort()) + "  ";
        vec.push_back(temp);
    }

    mLayout.UpdateUserListWithStringVec(vec);
}

void
CRClient::LogonFailed()
{
    /* TODO */
}

void
CRClient::HandleDrop(OperContext *ctx)
{
    /* TODO */
}

void
CRClient::HandleInput(char *input)
{
    bool parsed = false;
    Cmd * cmd = new Cmd();
    parsed = mInputParser.ParseInput(input, cmd);

    if (parsed) {
        if (cmd->IsInvalid()) {
            debug_log("invalid command " << input);
            /* invalid command, need to show warn info */
            mLayout.DisplaySystemMessage(cmd->GetErrStr());
#if 0
            OperContext *ctx = new OperContext(OperContext::OP_RECV);
            Msg *msg = new Msg;
            (*msg) << MsgType::local_warn_msg;
            (*msg) << cmd->GetErrStr();
            msg->SetLen();

            ctx->SetMessage(msg);
            ctx->SetConnID(-1);
            mNetService.Enqueue(ctx);
            OperContext::DecRef(ctx);
#endif
        } else {
            debug_log("valid command " << input);
            /* valid send cmd to server */
            OperContext *ctx = new OperContext(OperContext::OP_SEND);
            ctx->SetMessage(cmd->GetMsg());
            ctx->SetConnID(mConnId);
            mNetService.Enqueue(ctx);
            OperContext::DecRef(ctx);
        }

    } else {
        debug_log("normal chat message " << input);
        /* is a normal chat message */
        OperContext *ctx = new OperContext(OperContext::OP_SEND);
        Msg *msg = new Msg;
        (*msg) << MsgType::c2s_publish_chat_msg;
        (*msg) << mCurrentRoom.GetId();
        (*msg) << std::string(input);
        msg->SetLen();

        ctx->SetMessage(msg);
        ctx->SetConnID(mConnId);
        mNetService.Enqueue(ctx);
        OperContext::DecRef(ctx);
    }

    delete cmd;
    cmd = NULL;
}

void
CRClient::ReceiveChatMessage(Msg *msg)
{
    std::string fromName;
    std::string words;
    (*msg) >> fromName;
    (*msg) >> words;

    bool isSelf = (fromName == mUserName)? true : false;
    mLayout.ReceiveMessage(fromName, words, isSelf);
}

void
CRClient::SendLogon()
{
    OperContext *replyctx = new OperContext(OperContext::OP_SEND);              
    Msg *repmsg = new Msg();                                                    
    (*repmsg) << MsgType::c2s_logon;
    (*repmsg) << mUserName; 
    repmsg->SetLen();                                                           
    replyctx->SetMessage(repmsg);                                               
    replyctx->SetConnID(mConnId);                                                
    mNetService.Enqueue(replyctx);                                              
    OperContext::DecRef(replyctx);
}

void
CRClient::ReceiveLocalWarnMessage(Msg *msg)
{

}

void
CRClient::HandleCreateRoomRes(Msg *msg)
{
    int err = 0;
    std::string errstr;
    std::string roomName;
    (*msg) >> err;
    (*msg) >> errstr;
    (*msg) >> roomName;

    if (err = 0) {
        std::string info = "create room " + roomName + " succeed!";
        mLayout.DisplaySystemMessage(info);
    } else {
        std::string info = "create room " + roomName + " failed! reason: ";
        info += errstr;
        mLayout.DisplaySystemMessage(info);
    }
}







