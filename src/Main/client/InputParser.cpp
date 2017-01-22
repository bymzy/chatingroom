

#include "InputParser.hpp"
#include "include/Log.hpp"
#include "MsgType.hpp"
#include "Cmd.hpp"

bool
InputParser::ParseInput(char *input, Cmd *cmd)
{
    bool parsed = false;
    /* first non empty char is '/' ? */
    size_t len = strlen(input);
    uint32_t i = 0;

    do {
        while (input[i] == ' ' && i <len)
            ++i;

        if (i == len) {
            cmd->SetType(Cmd::CMD_null);
            /*empty message or all ' '*/
            debug_log("empty input:" << input);
            break;
        }

        cmd->SetType(Cmd::CMD_chat);

        if (input[i] == '/') {
            ++i;
            parsed = ParseCmdMessage(input + i, cmd);    
        }

    } while(0);

    return parsed;
}

bool
InputParser::ParseCmdMessage(char *input, Cmd *cmd)
{
    bool parsed = true;
    char ch = *input;
    switch (ch) 
    {
        case 'h':
            cmd->SetType(Cmd::CMD_help);
            cmd->SetInvalid(false);
            break;
        case 'c':
            parsed = ParseCreateRoom(input + 1, cmd);
            break;
        case 'j':
            parsed = ParseJoinRoom(input + 1, cmd);
            break;
        case 'w':
            parsed = ParseWhisper(input + 1, cmd);
            break;
        case 'r':
            parsed = ParseReply(input + 1, cmd);
            break;
        case 'q':
            parsed = ParseLeaveRoom(input + 1, cmd);
            break;
        case 'e':
            cmd->SetType(Cmd::CMD_exit);
            cmd->SetInvalid(false);
            break;
        case 'l':
            parsed = ParseList(input + 1, cmd);
            break;
        case '\0':
            parsed = false;
            break;
    }

    return parsed;
}

bool
InputParser::ParseCreateRoom(char *input, Cmd *cmd)
{
    bool parsed = false;
    char *index = input;

    /* waring: start end is [ ) */
    int namestart = 0;
    int nameend = 0;
    int roomlen = 0;

    int passwdstart = 0;
    int passwdend = 0;
    int passwdlen = 0;

    do {
        /* skip all space */
        while (*index == ' ') {
            ++index;
        }

        /* check if name is empty */
        if (*index == 0) {
            cmd->SetType(Cmd::CMD_create_room);
            cmd->SetErrStr("create room but empty room name (4-20 char length)");
            debug_log("create room command, but invalid name: " << (input + 1));
            break;
        }

        namestart = index - input;
        /* get room name */
        while (*index != ' ') {
            ++index;
        }
        nameend = index - input;
        roomlen = nameend - namestart;

        /* check room name length */
        if (roomlen < 4 || roomlen > 20) {
            cmd->SetType(Cmd::CMD_create_room);
            cmd->SetErrStr("room name at least 4 at most 20 chars length!");
            debug_log("create room command, but invalid name: " 
                    << (input + namestart));
            break;
        }

        /* skip all space */
        while (*index == ' ') {
            ++index;
        }

        /* get room passwd */
        passwdstart = index - input; 
        while (*index != ' ' && *index != 0) {
            ++index;
        }
        passwdend = index - input;
        
        passwdlen = passwdend - passwdstart;

        cmd->SetType(Cmd::CMD_create_room);
        if (passwdlen < 4 || passwdlen > 20) {
            cmd->SetErrStr("room passwd at least 4 at most 20 chars length!");
            debug_log("create room command, but invalid passwd: "
                    << (input + passwdstart));
            break;
        }

        parsed = true;

        std::string name;
        name.assign(input + namestart, nameend - namestart);

        std::string passwd;
        passwd.assign(input + passwdstart, passwdend - passwdstart);
        
        cmd->SetInvalid(false);
        cmd->SetLocalCmd(false);

        Msg *msg = new Msg;
        (*msg) << MsgType::c2s_create_room;
        (*msg) << name;
        (*msg) << passwd;
        msg->SetLen();
        cmd->SetMsg(msg);
        debug_log("create room command, name: " << name
                << ", passwd: " << passwd);

    } while(0);

    return parsed;
}

bool 
InputParser::ParseJoinRoom(char *input, Cmd *cmd)
{
    bool parsed = false;
    char *index = input;

    int roomlen = 0;
    int roombegin = 0;
    int roomend = 0;
    int passwdstart = 0;
    int passwdend = 0;
    int passwdlen = 0;

    do {
        while (*index == ' ') {
            ++index;
        }

        /* check if target is empty */
        if (*index == 0) {
            cmd->SetType(Cmd::CMD_null);
            cmd->SetErrStr("list command with no target");
            warn_log("list command invalid: " << input);
            break;
        }

        roombegin = index - input;
        /* get room name */
        while (*index != ' ' && *index != 0) {
            ++index;
        }
        roomend = index - input;
        roomlen = roomend - roombegin;

        std::string roomName;
        roomName.assign(input + roombegin, roomend - roombegin);

        if (roomlen < 4 || roomlen > 20) {
            cmd->SetType(Cmd::CMD_null);
            cmd->SetErrStr("join room ,but invalid room len!");
            debug_log("join room, but room name len invalid, name: " << input);
            break;
        }

        /* get passwd */
        /* skip all space */
        while (*index == ' ') {
            ++index;
        }

        /* get room passwd */
        passwdstart = index - input; 
        while (*index != ' ' && *index != 0) {
            ++index;
        }
        passwdend = index - input;
        passwdlen = passwdend - passwdstart;

        if (passwdlen < 4 || passwdlen > 20) {
            cmd->SetErrStr("room passwd at least 4 at most 20 chars length!");
            debug_log("join room command, but invalid passwd: "
                    << (input + passwdstart));
            break;
        }

        std::string passwd;
        passwd.assign(input + passwdstart , passwdend - passwdstart);

        parsed = true;
        Msg *msg = NULL;
        msg = new Msg;
        (*msg) << MsgType::c2s_join_room;
        (*msg) << roomName;
        (*msg) << passwd;
        msg->SetLen();

        cmd->SetLocalCmd(false);
        cmd->SetInvalid(false);
        cmd->SetMsg(msg);
        debug_log("join room command, with name: " << roomName);
        
    } while(0);

    return parsed;
}

bool 
InputParser::ParseWhisper(char *input, Cmd *cmd)
{
    bool parsed = false;
    char *index = input;

    std::string target;
    int targetbegin = 0;
    int targetend = 0;

    std::string words;
    int wordsbegin = 0;
    int wordsend = 0;

    do {
        while (*index == ' ') {
            ++index;
        }

        /* check if target is empty */
        if (*index == 0) {
            cmd->SetType(Cmd::CMD_null);
            cmd->SetErrStr("list command with no target");
            warn_log("list command invalid: " << input);
            break;
        }

        targetbegin = index - input;
        /* get user name */
        while (*index != ' ' && *index != 0) {
            ++index;
        }
        targetend = index - input;

        target.assign(input + targetbegin, targetend - targetbegin);

        while (*index == ' ') {
            ++index;
        }

        /* get words */
        wordsbegin = index - input; 
        while (*index != 0) {
            ++index;
        }
        wordsend = index - input;
        words.assign(input + wordsbegin, wordsend - wordsbegin);


        parsed = true;
        Msg *msg = new Msg;
        (*msg) << MsgType::c2s_whisper_msg;
        (*msg) << target;
        (*msg) << words;
        msg->SetLen();

        cmd->SetLocalCmd(false);
        cmd->SetInvalid(false);
        cmd->SetMsg(msg);

        debug_log("whisper command, target: " << target
                << ", words: " << words);
    } while(0);
    return parsed;
}

bool
InputParser::ParseReply(char *input, Cmd *cmd)
{
    return false;
}

bool
InputParser::ParseList(char *input, Cmd *cmd)
{
    bool parsed = false;
    char *index = input;
    int targetbegin = 0;
    int targetend = 0;

    do {
        while (*index == ' ') {
            ++index;
        }

        /* check if target is empty */
        if (*index == 0) {
            cmd->SetType(Cmd::CMD_null);
            cmd->SetErrStr("list command with no target");
            warn_log("list command invalid: " << input);
            break;
        }

        targetbegin = index - input;
        /* get room name */
        while (*index != ' ' && *index != 0) {
            ++index;
        }
        targetend = index - input;

        std::string target;
        target.assign(input + targetbegin, targetend - targetbegin);

        parsed = true;
        Msg *msg = NULL;
        if (target == "room") {
            cmd->SetType(Cmd::CMD_list_room);
            msg = new Msg;
            (*msg) << MsgType::local_list_room;
            msg->SetLen();
        } else {
            cmd->SetType(Cmd::CMD_null);
            cmd->SetErrStr("list command with invalid target");
            warn_log("list command invalid:" << target);
            break;
        }

        cmd->SetLocalCmd(true);
        cmd->SetInvalid(false);
        cmd->SetMsg(msg);
        debug_log("list command, with target: " << target);

    } while(0);

    return parsed;
}

bool
InputParser::ParseLeaveRoom(char *input, Cmd *cmd)
{
    bool parsed = false;
    char *index = input;

    do {
        if (*index != ' ' && *index != 0) {
            cmd->SetType(Cmd::CMD_null);
            warn_log("parse leave room command failed, input: " << input);
            break;
        }

        parsed = true;
        Msg *msg = new Msg;
        cmd->SetType(Cmd::CMD_leave_room);
        cmd->SetInvalid(false);
        cmd->SetLocalCmd(true);

        (*msg) << MsgType::local_leave_room;
        msg->SetLen();
        cmd->SetMsg(msg);

        debug_log("leave room command parsed succeed!");

    } while(0);

    return parsed;
}







