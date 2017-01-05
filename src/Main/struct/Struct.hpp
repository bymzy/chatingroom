

#ifndef __STRUCT_HPP__
#define __STRUCT_HPP__

#include <map>
#include <sstream>
#include "include/Msg.hpp"

#define HALL_ID 0

class User {
public:
    User():mPort(0), mID(0), mRoomId(0)
    {
    }

    User(std::string name, std::string ip, short port, uint64_t connId):
        mName(name), mIP(ip), mPort(port), mID(connId)
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


