

#ifndef __CR_CLIENT_HPP__
#define __CR_CLIENT_HPP__

#include <string.h>
#include "include/NetService.hpp"
#include "User.hpp"
#include "Layout.hpp"
#include "Room.hpp"
#include "InputParser.hpp"

class CRClient : public LogicService {
public:
    CRClient(std::string ip, short port, std::string user):
        LogicService("CRClient"),
        mIP(ip), mPort(port), mUserName(user),
        mNetService(this, ""), mLayout(this),
        mCurrentRoom(INVALID_ROOM_ID, "", ""), mConnId(0), mCdkRunning(true)
    {
    }

    ~CRClient()
    {
    }

public:
    void StartWindow()
    {
        mLayout.Start();
    }

    /* handle client input */
    void HandleInput(char *input);

    /* send logon info to server */ 
    void SendLogon();

    /* send message */
    void SendMessage(Msg *msg);

    bool IsCdkRunning() {
        return mCdkRunning;
    }


private:
    virtual int Init();
    virtual int Finit();
    virtual bool Process(OperContext *ctx);

private:
    void RecvMessage(OperContext *ctx);
    void HandleLogonRes(Msg *msg);
    void ParseRoomInfo(Msg *msg);
    void UpdateWindowUserList();
    void LogonFailed();
    void HandleDrop(OperContext *ctx);
    void ReceiveChatMessage(Msg *msg);
    void ReceiveLocalWarnMessage(Msg *msg);
    void HandleCreateRoomRes(Msg *msg);
    void HandleUpdateRoomList(Msg *msg);
    void HandleJoinRoomRes(Msg *msg);
    void ShowRoomList();
    void HandleLeaveRoom();
    void HandleWhisperMessage(Msg *msg);
    void HandleWhisperFailed(Msg *msg);
    void HandleLocalWhisperMsg(Msg *msg);
    void ShowHelpInfo();


private:
    /* server ip and port*/
    std::string mIP;
    short mPort;

    /* user name */
    std::string mUserName;
    User mUser;
    NetService mNetService; 
    Layout mLayout;

    map_id_room mRooms;

    Room mCurrentRoom;
    map_id_user mOnlines;

    /* connid to server */
    uint64_t mConnId;
    InputParser mInputParser;

    /* whether cdk need get input */
    bool mCdkRunning;
};


#endif


