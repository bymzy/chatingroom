

#ifndef __STRUCT_HPP__
#define __STRUCT_HPP__

#include <map>

#define HALL_ID 0

class User {
public:
    User(std::string name, std::string ip, short port, uint64_t connId):
        mName(name), mIP(ip), mPort(port), mID(connId)
    {
    }

    ~User()
    {
    }

public:
    void SetRoomId(uint32_t roomId)
    {
        mRoomId = roomId;
    }

public:
    std::string mName;
    std::string mIP;
    short mPort;
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


