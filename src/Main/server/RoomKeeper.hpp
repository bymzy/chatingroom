

#ifndef __STRUCT_ROOMKEEPER_HPP__
#define __STRUCT_ROOMKEEPER_HPP__

#include <stdint.h>
#include <set>
#include <string>
#include "include/Msg.hpp"
#include "Room.hpp"

class Worker;

class RoomKeeper {
public:
    RoomKeeper(Worker * worker):mRoomId(0), mWorker(worker)
    {
        std::string temp;
        CreateRoom(HALL_NAME, "", temp);
    }

    ~RoomKeeper()
    {
    }

public:
    int HandleLogon(const std::string& name, const std::string& ip, 
            const unsigned short& port,
            const uint64_t& userId, std::string& errstr);

    /* try to add user to room */
    int Join(const std::string& roomName, const uint64_t& userId,
            const std::string& passwd, std::string errstr);
    
    void GetRoomUserListMsg(uint32_t roomId, Msg* msg);

    int CreateRoom(const std::string& name, const std::string& passwd,
            std::string& errstr);

    void HandleDrop(uint64_t userId);

private:
    void SetUserRoomId(uint64_t userId, uint32_t roomId);

private:
    /* current allocated room id */
    uint32_t mRoomId;

    map_id_room mIdRoom;
    std::map<std::string, uint32_t> mRoomNames;

    map_id_user mIdUser;
    std::map<std::string, uint64_t> mUserNames;

    Worker *mWorker;
};


#endif


