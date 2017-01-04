

#include "Client.hpp"
#include "include/Log.hpp"
#include "MsgType.hpp"

int CRClient::Start()
{
    int err = 0;
    uint64_t connId;

    do {
        err = mNetService.Start();
        if (0 != err) {
            error_log("netservice start failed!");
            break;
        }

        err = mNetService.StartConnectRemote(mIP, mPort, connId);
        if (0 != err) {
            error_log("client connect remote failed! ip: " << mIP 
                    << ", port: " << mPort);
            break;
        }

        {
            OperContext *replyctx = new OperContext(OperContext::OP_SEND);              
            Msg *repmsg = new Msg();                                                    
            (*repmsg) << MsgType::c2s_logon;
            (*repmsg) << mUser; 
            repmsg->SetLen();                                                           
            replyctx->SetMessage(repmsg);                                               
            replyctx->SetConnID(connId);                                                
            mNetService.Enqueue(replyctx);                                              
            OperContext::DecRef(replyctx);
        }


    } while(0);

    return err;
}


