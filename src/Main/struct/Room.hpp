

#ifndef __STRUCT_ROOM_HPP__
#define __STRUCT_ROOM_HPP__

#include <set>
#include <map>
#include <stdint.h>
#include "User.hpp"

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


