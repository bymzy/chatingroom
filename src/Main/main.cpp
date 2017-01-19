

#include <iostream>
#include <time.h>
#include "include/common.h"
#include "client/Client.hpp"
#include "server/Server.hpp"
#include "include/Log.hpp"
#include "include/ThreadLogger.hpp"

ThreadLogger *g_logger = NULL;

int RunAsServer()
{
    int err = 0;
    CRServer server("127.0.0.1", 2141);
    err = server.Start();

    return err;
}

int RunAsClient()
{
    int err = 0;
    time_t now = time(NULL);

    CRClient client("127.0.0.1", 2141, i2s(now));
    err = client.Start();
    if (0 != err) {
        goto OUT;
    }

    client.StartWindow();

    /* stop client */
    client.Stop();

OUT:
    return err;
}

int main(int argc, char *argv[])
{
    int err = 0;
    bool isServer = false;
    if (argc != 2) {
        std::cerr << "invalid args" << std::endl;
        err = EINVAL;
        goto EXIT;
    }

    if (strcmp("c", argv[1]) == 0) {
        isServer = false;
    } else if (strcmp("s", argv[1]) == 0) {
        isServer = true;
    } else {
        std::cerr << "invalid role" << std::endl;
        err = EINVAL;
        goto EXIT;
    }


    if (isServer) {
        g_logger = new ThreadLogger("CRServer");
        g_logger->Start();
        err = RunAsServer();
    } else {
        g_logger = new ThreadLogger("CRClient");
        g_logger->Start();
        err = RunAsClient();
    }

    g_logger->Stop();
EXIT:
    return err;
}


