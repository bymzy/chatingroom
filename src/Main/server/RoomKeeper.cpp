
#include "assert.h"
#include "Err.hpp"
#include "include/Log.hpp"
#include "include/Context.hpp"
#include "MsgType.hpp"
#include "RoomKeeper.hpp"
#include "Worker.hpp"

int
RoomKeeper::HandleLogon(const std::string& name, const std::string& ip,
        const unsigned short& port, 
        const uint64_t& userId, std::string& errstr, User** retuser)
{
    int err = 0;
    iter_id_user iter;
    User *user = NULL;

    do {
        /* find if this conn already logoned */
        iter = mIdUser.find(userId);
        if (mIdUser.end() != iter) {
            warn_log("user has logoned before!");
            err = Err::user_name_alredy_logoned;
            errstr = "user has logoned before!";
            break;
        }

        /* find if someone has used this name */
        if (mUserNames.find(name) != mUserNames.end()) {
            warn_log("user with same name exists!");
            err = Err::user_same_name_exist;
            errstr = "user with sanme name exists on server!";
            break;
        }

        /* create User and insert */
        user = new User(name, ip, port, userId);
        mIdUser.insert(std::make_pair(userId, user));
        mUserNames.insert(std::make_pair(name, userId));

        *retuser = user;

        /* join hall */
        err = JoinRoom(HALL_NAME, userId, "", errstr);
        assert(err == 0);

    } while(0);

    return err;
}

int 
RoomKeeper::JoinRoom(const std::string& roomName, const uint64_t& userId,
        const std::string& passwd, std::string& errstr)
{
    int err = 0;
    std::map<std::string, uint32_t>::iterator room_name_id_iter;
    uint32_t roomId;
    iter_id_room id_room_iter;
    Room *room = NULL;

    do {
        /* check if room exists*/
        room_name_id_iter = mRoomNames.find(roomName);
        if (room_name_id_iter == mRoomNames.end()) {
            warn_log("room" << roomName << " not exist!");
            err = Err::room_not_exist;
            errstr = "room not exists!";
            break;
        }

        roomId = room_name_id_iter->second;
        id_room_iter = mIdRoom.find(roomId);
        assert(id_room_iter != mIdRoom.end());
        room = id_room_iter->second;

        /* check password */
        if (!room->IsPasswdOkay(passwd)) {
            warn_log("passwd not match ");
            err = Err::room_passwd_not_match;
            errstr = "room passwd not match, try again!";
            break;
        }
        
        /* check if user already exist in this room */
        if (room->IsUserIn(userId)) {
            warn_log("user already in this room");
            err = Err::user_already_in_this_room;
            errstr = "user already in this room";
            break;
        }

        /* add user to this room */
        room->Add(userId);

        /* set user room id */
        SetUserRoomId(userId, roomId);

        /* publish room info */
        PublishRoomInfo(room);

        debug_log("user " << userId 
                << " join room " << roomName << " success!");
    } while(0);

    return err;
}

void
RoomKeeper::SetUserRoomId(uint64_t userId, uint32_t roomId)
{
    iter_id_user iter = mIdUser.find(userId);
    assert(iter != mIdUser.end());
    User *user = iter->second;
    user->SetRoomId(roomId);
}

void
RoomKeeper::GetRoomInfoMsg(uint32_t roomId, Msg *msg)
{
    std::set<uint64_t> users;
    std::set<uint64_t>::iterator iter_users;
    uint32_t count = 0;
    User *user = NULL;
    iter_id_user id_user_iter;


    /* get all user of this room*/
    iter_id_room iter = mIdRoom.find(roomId);
    assert(iter != mIdRoom.end());
    Room *room = iter->second; 
    users = room->GetAllUsers();

    (*msg) << MsgType::s2c_room_info;

    /* encode room info */
    room->Encode(msg);
    debug_log("GetRoomInfoMsg room info " << room->DebugString());

    /* encode msg with user info */
    count = users.size();
    (*msg) << count;

    iter_users = users.begin();
    for (;iter_users != users.end(); ++iter_users) {
        id_user_iter = mIdUser.find(*iter_users);
        assert(id_user_iter != mIdUser.end());
        user = id_user_iter->second;
        user->Encode(msg);
        debug_log("user info " << user->DebugString());
    }
}

int
RoomKeeper::CreateRoom(const std::string& name, const std::string& passwd,
        std::string& errstr)
{
    int err = 0;
    std::map<std::string, uint32_t>::iterator found;
    Room *room = NULL;

    do {
        found = mRoomNames.find(name);
        if (found != mRoomNames.end()) {
            debug_log("room " << name << " already created");
            err = Err::room_with_same_name_already_exists;
            errstr = "room with same name already exists";

            debug_log("create room: " << name << " failed"
                    << ", errstr: " << errstr);
            break;
        }

        /* TODO check room name valid? */

        room = new Room(mRoomId, name, passwd);
        mRoomNames.insert(std::make_pair(name, mRoomId));
        mIdRoom.insert(std::make_pair(mRoomId, room));

        ++mRoomId;

        debug_log("create room: " << name << " succeed"
                << ", room id: " << mRoomId -1);
    } while(0);

    return err;
}

void
RoomKeeper::HandleDrop(uint64_t userId)
{
    iter_id_user found;
    iter_id_room roomFound;
    User *user = NULL;
    Room *room = NULL;
    uint32_t roomId;

    do {
        found = mIdUser.find(userId);
        if (found == mIdUser.end()) {
            warn_log("HandleDrop but userid not found");
            break;
        }

        user = found->second;
        roomId = user->GetRoomId();

        roomFound = mIdRoom.find(roomId);
        assert(roomFound != mIdRoom.end());
        room = roomFound->second;

        /* delete from room 
         * inform roomers that he dropped 
         * */
        room->HandleUserLeave(userId);

        /* delete from mIdUser */
        mUserNames.erase(user->GetName());
        mIdUser.erase(userId);

        debug_log("user " << user->GetName()
                << ", user id: " << userId
                << ", dropped from room: " << room->GetName()
                << ", room id: " << room->GetId())

        delete user;
        user = NULL;

        /* info user left */
        PublishRoomInfo(room);
    } while(0);

    return;
}

void
RoomKeeper::PublishRoomInfo(Room *room)
{
    std::set<uint64_t> users;
    std::set<uint64_t>::iterator iter_users;
    
    Msg msg;
    GetRoomInfoMsg(room->GetId(), &msg);
    msg.SetLen();

    /**/
    users = room->GetAllUsers();
    iter_users = users.begin();
    for (;iter_users != users.end(); ++iter_users) {
        SendMessage(*iter_users, msg.Dup());
    }
}

void
RoomKeeper::SendMessage(uint64_t connId, Msg *msg)
{
    OperContext *ctx = new OperContext(OperContext::OP_SEND);
    ctx->SetMessage(msg);
    ctx->SetConnID(connId);
    mWorker->Enqueue(ctx);
    OperContext::DecRef(ctx);
}

void
RoomKeeper::PublishRoomMessage(uint32_t roomId, uint64_t userId, const std::string& input)
{
    User * user = GetUserById(userId);
    Room * room = GetRoomById(roomId);
    User * temp = NULL;
    std::set<uint64_t> users;
    std::set<uint64_t>::iterator user_index;

    Msg msg;
    msg << MsgType::s2c_room_chat_msg;
    msg << user->GetName();
    msg << input;
    msg.SetLen();

    debug_log("receive room chat message: " << user->GetName() << " " << input
            << ", room id: " << roomId
            << ", room name: " << room->GetName());

    users = room->GetAllUsers();
    user_index = users.begin();

    for (;user_index != users.end(); ++user_index) {
        temp = GetUserById(*user_index);
        SendMessage(temp->GetId(), msg.Dup()); 
    }
}

void
RoomKeeper::PublishRoomList(uint64_t connId)
{
    Room *room = NULL;
    User *user = NULL;
    uint32_t count = mIdRoom.size();
    Msg msg;
    msg << MsgType::s2c_room_list;
    msg << count;

    debug_log("publish room list cout: " << mIdRoom.size());
    iter_id_room iter = mIdRoom.begin();
    for (;iter != mIdRoom.end(); ++iter) {
        room = iter->second;
        room->Encode(&msg);
        debug_log("room info: " << room->DebugString());
    }
    msg.SetLen();

    if (connId != 0) {
        SendMessage(connId, msg.Dup());
    } else {
        /* send to all users */
        iter_id_user ituser = mIdUser.begin();
        for (;ituser != mIdUser.end(); ++ituser) {
            user = ituser->second;
            SendMessage(user->GetId(), msg.Dup());
        }
    } 
}

User *
RoomKeeper::GetUserById(uint64_t userId)
{
    iter_id_user index;
    index = mIdUser.find(userId);
    assert(index != mIdUser.end());
    
    return index->second;
}

User *
RoomKeeper::GetUserByName(std::string name)
{
    std::map<std::string ,uint64_t>::iterator iter;
    iter = mUserNames.find(name);
    if (iter == mUserNames.end()) {
        return NULL;
    }

    iter_id_user iter2;
    iter2 = mIdUser.find(iter->second);
    if (iter2 == mIdUser.end()) {
        return NULL;
    }

    return iter2->second;
}

Room *
RoomKeeper::GetRoomById(uint32_t roomId)
{
    iter_id_room index;
    index = mIdRoom.find(roomId);
    assert(index != mIdRoom.end());

    return index->second;
}

int
RoomKeeper::HandleJoinRoom(const std::string& roomName, const std::string& passwd,
        uint64_t userId, std::string& errstr)
{
    int err = 0;
    /* check room exists */
    iter_id_user userIter;
    iter_id_room roomIter;
    uint32_t oldRoomId;
    User *user = NULL;
    Room *oldRoom = NULL;

    do {
        /* get where user used to belong to */
        userIter = mIdUser.find(userId);
        if (userIter == mIdUser.end()) {
            err = Err::user_not_exist;
            errstr = "join room ,but user not exist!";
            warn_log("join room ,but user not exist!");
            break;
        }
        user = userIter->second;

        oldRoomId = user->GetRoomId();
        roomIter = mIdRoom.find(oldRoomId);
        assert(roomIter != mIdRoom.end());
        oldRoom = roomIter->second;

        /* join new room */
        err = JoinRoom(roomName, userId, passwd, errstr);
        if (err != 0) {
            break;
        }

        /* leave old room */
        err = LeaveRoom(oldRoom->GetName(), userId, errstr);
        assert(err == 0);

    } while(0);

    return err;
}

int
RoomKeeper::LeaveRoom(const std::string& roomName, uint64_t userId, 
        std::string& errstr)
{
    int err = 0;
    std::map<std::string, uint32_t>::iterator room_name_id_iter;
    uint32_t roomId;
    iter_id_room id_room_iter;
    Room *room = NULL;

    do {
        /* check if room exists*/
        room_name_id_iter = mRoomNames.find(roomName);
        if (room_name_id_iter == mRoomNames.end()) {
            warn_log("room" << roomName << " not exist!");
            err = Err::room_not_exist;
            errstr = "room not exists!";
            break;
        }

        roomId = room_name_id_iter->second;
        id_room_iter = mIdRoom.find(roomId);
        assert(id_room_iter != mIdRoom.end());
        room = id_room_iter->second;

        room->HandleUserLeave(userId);
        PublishRoomInfo(room);

        debug_log("leave room, room name: " << roomName
                << ", user id: " << userId);
    } while(0);

    return err;
}






