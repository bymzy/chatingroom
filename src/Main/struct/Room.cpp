

#include "Room.hpp"
#include "include/Log.hpp"

bool
Room::IsPasswdOkay(const std::string& passwd)
{
    debug_log("right passwd:" << mPassWord
            << ", requeset passwd: " << passwd);
    return (mPassWord == passwd);
}

bool
Room::IsUserIn(uint64_t userId)
{
    std::set<uint64_t>::iterator iter = mUsers.end();

    iter = mUsers.find(userId);
    
    return (iter==mUsers.end())? false: true;
}

void
Room::Add(uint64_t userId)
{
    if (IsUserIn(userId)) {
        return;
    }

    mUsers.insert(userId);

    /* TODO need to update user list of this room */
    return;
}

void
Room::HandleUserLeave(uint64_t userId)
{
    if (!IsUserIn(userId)) {
        return;
    }

    mUsers.erase(userId);

    /* TODO need to update user list of this room */
}


