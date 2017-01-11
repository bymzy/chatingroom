

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
            debug_log("invalid input:" << input);
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
            cmd->SetType(Cmd::CMD_exit);
            break;
        case 'e':
            cmd->SetType(Cmd::CMD_exit);
            break;
        case 'i':
            cmd->SetType(Cmd::CMD_info);
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
    size_t roomlen = 0;
    char room[21];

    do {
        if (*input != ' ') {
            break;
        }

        parsed = true;
        roomlen = strlen(input + 1);
        /* parse room name */
        if (roomlen < 4 || roomlen > 20) {
            cmd->SetInvalid(true);
            cmd->SetType(Cmd::CMD_create_room);
            cmd->SetErrStr("room name at least 4 at most 20 chars length!");
            debug_log("create room, but invalid name: " << (input + 1));
        } else {
            memset(room, 0, 21);
            strcpy(room, input + 1);
            Msg *msg = new Msg;
            (*msg) << MsgType::c2s_create_room;
            (*msg) << room; 
            msg->SetLen();
            cmd->SetMsg(msg);
        }
    } while(0);
}

bool 
InputParser::ParseJoinRoom(char *input, Cmd *cmd)
{
}

bool 
InputParser::ParseWhisper(char *input, Cmd *cmd)
{

}

bool
InputParser::ParseReply(char *input, Cmd *cmd)
{
}




