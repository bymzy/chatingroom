

#ifndef __STRUCT_ROOM_HPP__
#define __STRUCT_ROOM_HPP__

#include <set>
#include <map>
#include <stdint.h>
#include <sstream>
#include "User.hpp"
#include "include/Msg.hpp"

class Room {
public:
    Room(uint32_t id, std::string name, std::string passwd):
        mID(id), mName(name), mPassWord(passwd)
    {
    }

    ~Room()
    {
    }

public:
    bool IsPasswdOkay(const std::string& passwd);
    bool IsUserIn(uint64_t userId);
    void Add(uint64_t userId);
    void HandleUserLeave(uint64_t userId);

    std::set<uint64_t> GetAllUsers() 
    {
        return mUsers;
    }

    void SetId(uint32_t id) 
    {
        mID = id;
    }

    void Encode(Msg *msg)
    {
        uint32_t count = mUsers.size();
        std::set<uint64_t>::iterator iter = mUsers.begin();
        
        (*msg) << mID;
        (*msg) << mName;
        (*msg) << count;

        for (;iter != mUsers.end(); ++iter) {
            (*msg) << *iter;
        }
    }

    void Decode(Msg *msg)
    {
        uint32_t count = 0;
        uint64_t userId = 0;

        (*msg) >> mID;
        (*msg) >> mName;
        (*msg) >> count;

        for (uint32_t i = 0; i < count; ++i) {
            (*msg) >> userId;
            mUsers.insert(userId);
        }
    }

    std::string DebugString()
    {
        std::stringstream ss;
        ss << " room name: " << mName
            << " room id: " << mID;
        return ss.str();
    }

private:
    uint32_t mID;
    std::string mName;
    std::string mPassWord;
    /* users in this room */
    std::set<uint64_t> mUsers;
};

typedef std::map<uint32_t, Room*> map_id_room;
typedef std::map<uint32_t, Room*>::iterator iter_id_room;

#endif


