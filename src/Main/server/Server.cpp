

#include "Server.hpp"
#include "include/Log.hpp"

int CRServer::Start()
{
    int err = 0;

    do {
        err = mWorker.Start();
        if (0 != err) {
            error_log("start worker failed!");
            break;
        }

        err = mNetService.AddListener(mIP, mPort);
        if (0 != err) {
            error_log("add listener failed, err: " << err);
            break;
        }
        mNetService.StartWithoutNewThread();
    } while(0);

    return err;
}

void 
CRServer::SendMessage(OperContext *ctx)
{
    mNetService.Enqueue(ctx);
}


