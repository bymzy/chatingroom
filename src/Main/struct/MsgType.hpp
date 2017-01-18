

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

        /* 4 bytes: msg type
         * string: room name
         * */
        c2s_create_room,

        /* 4 bytes: msg type
         * 4 byts: errno
         * string: errstr
         * string: room name
         * */
        s2c_create_room_res,

        /* 4 bytes: msg type
         * 4 bytes: room count
         * repeated: room info
         * */
        s2c_room_list,

        /* 4 bytes: msg type
         * string: room name
         * string: passwd
         * */
        c2s_join_room,

        /* 4 bytes: msg type
         * 4 bytes: errno
         * 4 bytes: errstr
         * */
        s2c_join_room_res,


        /**/
        /* begin of local msg */
        local_msg_begin,

        /* 4 bytes: msg type
         * string: warn info
         * */
        local_warn_msg,

        /* 4 bytes: msg type*/
        local_list_room,

        /* end of local msg */
        local_msg_end,
    };
};



#endif


