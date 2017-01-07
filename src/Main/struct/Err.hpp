

#ifndef __MSG_TYPE_HPP__
#define __MSG_TYPE_HPP__

class Err {
public:
    enum _err{
        user_name_alredy_logoned = 1 ,   
        user_same_name_exist,
        room_not_exist,
        room_passwd_not_match,
        user_already_in_this_room,
        room_with_same_name_already_exists,
    };
};


#endif


