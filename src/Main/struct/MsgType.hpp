

#ifndef __STRUCT_MSG_TYPE_HPP__
#define __STRUCT_MSG_TYPE_HPP__

class MsgType {
public:
    enum _type{
        /* 4 bytes: msg type
         * string: user name
         * */
        c2s_logon = 1,

        /* 4 bytes: msg type
         * 4 bytes: errno
         * string: errstr
         * */
        s2c_logon_res,

        /* 4 bytes: msg type
         * 4 bytes: user count
         * repeated: user infos
         * */
        s2c_room_info, 
    };
};




#endif


