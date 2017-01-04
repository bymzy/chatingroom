

#include <iostream>

#include "client/Layout.hpp"
#include "client/Client.hpp"
#include "server/Server.hpp"

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

    CRClient client("127.0.0.1", 2141, "test");
    Layout lay(&client);

    err = client.Start();
    if (0 != err) {
        goto OUT;
    }

    err = lay.Start();

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
        err = RunAsServer();
    } else {
        err = RunAsClient();
    }

EXIT:
    return err;
}


