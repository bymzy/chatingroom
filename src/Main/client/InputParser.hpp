

/* command begin with '/'
 * command list:
 * /h            show help info
 * /c room       create room 
 * /j room       join room    
 * /w username msg whisper to user 
 * /r username msg reply to user
 * /q            exit room, exit HALL--> exit program 
 * /e         exit program
 * /i         show current info
 * */

#ifndef __CLIENT_INPUT_PARSER_HPP__
#define __CLIENT_INPUT_PARSER_HPP__

#include "Cmd.hpp"

class InputParser {
public:
    InputParser()
    {
    }
    ~InputParser()
    {
    }

public:
    bool ParseInput(char *input, Cmd *cmd);
    
private:
    bool ParseCmdMessage(char *input, Cmd *cmd);
    bool ParseCreateRoom(char *input, Cmd *cmd);
    bool ParseJoinRoom(char *input, Cmd *cmd);
    bool ParseWhisper(char *input, Cmd *cmd);
    bool ParseReply(char *input, Cmd *cmd);
};

#endif




