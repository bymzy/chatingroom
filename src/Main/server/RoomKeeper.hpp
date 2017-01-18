

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
            const uint64_t& userId, std::string& errstr, User **user);

    /* try to add user to room */
    int JoinRoom(const std::string& roomName, const uint64_t& userId,
            const std::string& passwd, std::string& errstr);
    
    void GetRoomInfoMsg(uint32_t roomId, Msg* msg);

    int CreateRoom(const std::string& name, const std::string& passwd,
            std::string& errstr);

    int LeaveRoom(const std::string& name, uint64_t userId, 
            std::string& errstr);

    int HandleJoinRoom(const std::string& roomName, const std::string& passwd,
            uint64_t userId, std::string& errstr);

    void HandleDrop(uint64_t userId);

    void PublishRoomInfo(Room *room);

    void SendMessage(uint64_t connId, Msg *msg);

    void PublishRoomMessage(uint32_t roomId, uint64_t userId, const std::string& msg);

    void PublishRoomList(uint64_t connId = 0);


    User * GetUserById(uint64_t userId);

    Room * GetRoomById(uint32_t roomId);

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


