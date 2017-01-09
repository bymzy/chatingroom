

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
        s2c_logon_res = 2,

        /* 4 bytes: msg type
         * 4 bytes: user count
         * repeated: user infos
         * */
        s2c_room_info = 3, 

        /* 4 bytes: msg type
         * 4 bytes: room id
         * string: chat msg
         * */
        c2s_publish_chat_msg = 4,

        /* 4 bytes: msg type
         * string: from user name
         * string: chat msg*/
        s2c_room_chat_msg = 5,
    };
};



#endif


