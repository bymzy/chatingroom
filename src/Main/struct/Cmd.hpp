

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
        CMD_leave_room,
        CMD_exit,

        /* local command */
        CMD_list_room,
        CMD_list_user,
        CMD_help,

        CMD_chat,
        CMD_null,
    }CmdType;

public:
    Cmd(): mType(CMD_null), mMsg(NULL), mInvalid(true), mLocalCmd(true)
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

    bool IsInvalid()
    {
        return mInvalid;
    }

    void SetErrStr(const std::string & str)
    {
        mErrStr = str;
    }

    std::string GetErrStr()
    {
        return mErrStr;
    }

    void SetLocalCmd(bool local)
    {
        mLocalCmd = local;
    }

    bool IsLocalCmd()
    {
        return mLocalCmd;
    }

private:
    CmdType mType;
    Msg *mMsg;
    /* command invalid reason */
    std::string mErrStr;
    bool mInvalid;
    bool mLocalCmd;
};


#endif


