

#ifndef __STRUCT_USER_HPP__
#define __STRUCT_USER_HPP__

#include <map>
#include <sstream>
#include "include/Msg.hpp"

#define HALL_ID 0
#define HALL_NAME "HALL"
#define INVALID_ROOM_ID (uint32_t(-1))

class User {
public:
    User():mPort(0), mID(0), mRoomId(INVALID_ROOM_ID)
    {
    }
    User(std::string name, std::string ip, short port, uint64_t connId):
        mName(name), mIP(ip), mPort(port),
        mID(connId), mRoomId(INVALID_ROOM_ID)
    {
    }

    ~User()
    {
    }

public:
    std::string DebugString()
    {
        std::stringstream ss;
        ss << mName <<", " << mIP << ", "
           << mPort <<", " << mID << ", "
           << mRoomId;
        return ss.str();
    }

    void Encode(Msg * msg)
    {
        (*msg) << mName;
        (*msg) << mIP;
        (*msg) << mPort;
        (*msg) << mID;
        (*msg) << mRoomId;
    }

    void Decode(Msg *msg)
    {
        (*msg) >> mName;
        (*msg) >> mIP;
        (*msg) >> mPort;
        (*msg) >> mID;
        (*msg) >> mRoomId;
    }

    void SetRoomId(uint32_t roomId)
    {
        mRoomId = roomId;
    }

    uint32_t GetRoomId() 
    {
        return mRoomId;
    }

    std::string GetName()
    {
        return mName;
    }

    std::string GetIp()
    {
        return mIP;
    }

    uint64_t GetId()
    {
        return mID;
    }

    unsigned short GetPort()
    {
        return mPort;
    }

public:
    std::string mName;
    std::string mIP;
    unsigned short mPort;
    /* set conn id as user id */
    uint64_t mID;
    /* room id if it is in room
     * 0 ---> HALL
     * n ---> ROOM
     * */
    uint32_t mRoomId;
};

typedef std::map<uint64_t, User*> map_id_user;
typedef std::map<uint64_t, User*>::iterator iter_id_user;

#endif


