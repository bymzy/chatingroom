

#ifndef __STRUCT_CMD_HPP__
#define __STRUCT_CMD_HPP__

#include "include/Msg.hpp"

class Cmd {
public:
    typedef enum _type {
        /* remote command */
        CMD_create_room,
        CMD_join_room,
        CMD_whisper,
        CMD_quit,
        CMD_exit,

        /* local command */
        CMD_info,
        CMD_help,

        CMD_chat,
        CMD_null,
    }CmdType;

public:
    Cmd(): mType(CMD_null), mMsg(NULL), mInvalid(false)
    {
    }
    ~Cmd()
    {
    }

public:
    Msg *GetMsg()
    {
        return mMsg;
    }

    CmdType GetType()
    {
        return mType;
    }

    void SetType(CmdType type)
    {
        mType = type;
    }

    void SetMsg(Msg *msg)
    {
        mMsg = msg;
    }

    void SetInvalid(bool invalid)
    {
        mInvalid = invalid;
    }

    void SetErrStr(const std::string & str)
    {
        mErrStr = str;
    }

private:
    CmdType mType;
    Msg *mMsg;
    /* command invalid reason */
    std::string mErrStr;
    bool mInvalid;
};


#endif


