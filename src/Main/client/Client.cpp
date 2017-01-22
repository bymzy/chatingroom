

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
        case MsgType::s2c_room_list:
            HandleUpdateRoomList(msg);
            break;
        case MsgType::s2c_join_room_res:
            HandleJoinRoomRes(msg);
            break;
        case MsgType::local_list_room:
            ShowRoomList();
            break;
        case MsgType::local_leave_room:
            HandleLeaveRoom();
            break;
        case MsgType::s2c_whisper_msg:
            HandleWhisperMessage(msg);
            break;
        case MsgType::s2c_whisper_msg_failed:
            HandleWhisperFailed(msg);
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
    mNetService.Stop();
    debug_log("CRClient finit!");

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
    mUser.SetRoomId(mCurrentRoom.GetId());
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
    /* warning this is done in main thread*/

    bool parsed = false;
    Cmd * cmd = new Cmd();
    parsed = mInputParser.ParseInput(input, cmd);

    do {
        if (parsed) {
            if (cmd->IsInvalid()) {
                debug_log("invalid command " << input);
                /* invalid command, need to show warn info */
                mLayout.DisplaySystemMessage(cmd->GetErrStr());
                break;
            }  

            debug_log("valid command " << input
                    << ", local: " << cmd->IsLocalCmd());

            if (cmd->IsLocalCmd()) {
                if ((cmd->GetType() == Cmd::CMD_leave_room 
                        && mCurrentRoom.GetName() == HALL_NAME) ||
                        cmd->GetType() == Cmd::CMD_exit) {
                    mCdkRunning = false;
                    break;
                }

                if (cmd->GetType() == Cmd::CMD_help) {
                    ShowHelpInfo();
                    break;
                }

                /* valid local cmd */
                OperContext *ctx = new OperContext(OperContext::OP_RECV);
                ctx->SetMessage(cmd->GetMsg());
                ctx->SetConnID(mConnId);
                mNetService.Enqueue(ctx);
                OperContext::DecRef(ctx);

            } else { 
                /* valid remote cmd to server */
                SendMessage(cmd->GetMsg());
            }
        } else {
            /* is a normal chat message */
            Msg *msg = new Msg;
            (*msg) << MsgType::c2s_publish_chat_msg;
            (*msg) << mCurrentRoom.GetId();
            (*msg) << std::string(input);
            msg->SetLen();

            SendMessage(msg);
            debug_log("normal chat message " << input
                    << ", room id: " << mCurrentRoom.GetId());
        }

    } while (0);

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

    debug_log("HandleCreateRoomRes, err: " << err
            << ", errstr: " << errstr
            << ", roomName: " << roomName);

    if (err == 0) {
        std::string info = "create room " + roomName + " succeed!";
        mLayout.DisplaySystemMessage(info);
    } else {
        std::string info = "create room " + roomName + " failed! reason: ";
        info += errstr;
        mLayout.DisplaySystemMessage(info);
    }
}

void
CRClient::HandleUpdateRoomList(Msg *msg)
{
    Room *room = NULL;
    uint32_t count = 0;
    (*msg) >> count;

    iter_id_room iter = mRooms.begin();
    /* clear all current rooms */
    for (;iter != mRooms.end(); ++iter) {
        delete iter->second;
    }
    mRooms.clear();

    debug_log("room count: " << count);
    for (uint32_t i = 0; i < count; ++i) {
        room = new Room;
        room->Decode(msg);
        debug_log("UpdateRoomList, room info: " << room->DebugString());
        mRooms.insert(std::make_pair(room->GetId(), room));
    }
}

void
CRClient::ShowRoomList()
{
    std::stringstream ss;
    ss << "Total room count: " << mRooms.size();
    mLayout.DisplaySystemMessage(ss.str());
    ss.clear();

    iter_id_room iter = mRooms.begin();
    for (; iter != mRooms.end(); ++iter) {
        mLayout.DisplaySystemMessage(iter->second->DebugString());
    }
}

void
CRClient::HandleJoinRoomRes(Msg *msg)
{
    int err = 0;
    std::string errstr;
    std::stringstream ss;
    std::string roomName;

    (*msg) >> err;
    (*msg) >> errstr;
    (*msg) >> roomName;

    debug_log("handle join room res, error: " << err
            << ", errstr: " << errstr);

    if (err != 0) {
        ss << "join room " << roomName << " failed: " <<  errstr;
        mLayout.DisplaySystemMessage(ss.str());
    } else {
        ss << "join room "<< roomName << " success!";
        mLayout.DisplaySystemMessage(ss.str());
    }
}

void
CRClient::HandleWhisperMessage(Msg *msg)
{
    std::string from, words;
    std::stringstream ss;

    (*msg) >> from;
    (*msg) >> words;

    debug_log("handle whisper message from: " << from
            << ", words: " << words);

    ss << " whisper from " << from
        << ": " << words;

    mLayout.DisplaySystemMessage(ss.str());
}

void
CRClient::HandleLeaveRoom()
{
    /* if current is in HALL, then exist client
     * else join HALL
     * */

    if (mCurrentRoom.GetName() == HALL_NAME) {
        mCdkRunning = false;
        info_log("levaving CR client!");
    } else {
        /* join HALL */
        Msg *msg = new Msg;
        (*msg) << MsgType::c2s_join_room;
        (*msg) << HALL_NAME;
        (*msg) << "";
        msg->SetLen();
        SendMessage(msg);
    }
}

void
CRClient::SendMessage(Msg *msg)
{
    OperContext *ctx = new OperContext(OperContext::OP_SEND);
    ctx->SetMessage(msg);
    ctx->SetConnID(mConnId);
    mNetService.Enqueue(ctx);
    OperContext::DecRef(ctx);
}

void
CRClient::HandleWhisperFailed(Msg *msg)
{
    std::string to;
    std::string reason;
    std::stringstream ss;

    (*msg) >> to;
    (*msg) >> reason;

    ss << "send whisper to " << to << " failed, reason: " << reason;

    mLayout.DisplaySystemMessage(ss.str());
}

void
CRClient::ShowHelpInfo()
{
    mLayout.DisplaySystemMessage("CRClient currently support this commands");
    mLayout.DisplaySystemMessage(" command list:");
    mLayout.DisplaySystemMessage(" /h            show help info\n");
    mLayout.DisplaySystemMessage(" /c room       create room");
    mLayout.DisplaySystemMessage(" /j room       join room\n");    
    mLayout.DisplaySystemMessage(" /w username msg whisper to user");
    mLayout.DisplaySystemMessage(" /q            exit room, exit HALL--> exit program");
    mLayout.DisplaySystemMessage(" /e         exit program");
    mLayout.DisplaySystemMessage(" /i         show current info");
    mLayout.DisplaySystemMessage(" /l [room,user] list room or user info");
}


