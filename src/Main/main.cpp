

#include <iostream>
#include <time.h>
#include "include/common.h"
#include "client/Client.hpp"
#include "server/Server.hpp"
#include "include/Log.hpp"
#include "include/ThreadLogger.hpp"
#include "include/cmdline.h"
#include "include/common.h"
#include <unistd.h>

ThreadLogger *g_logger = NULL;

int RunAsServer(std::string ip, short port)
{
    int err = 0;
    CRServer server(ip, port);
    err = server.Start();

    return err;
}

int RunAsClient(std::string ip, short port, std::string user)
{
    int err = 0;
    time_t now = time(NULL);

    CRClient client(ip, port, user);
    err = client.Start();
    if (0 != err) {
        goto OUT;
    }

    client.StartWindow();

OUT:
    return err;
}

int main(int argc, char *argv[])
{
    int err = 0;
    cmdline::parser parser;
    std::string role;
    short port;
    std::string ip;
    std::string user;
    bool isServer = false;
    bool isDaemon = false;

    parser.add<std::string>("role", 'r', "CR role, server or client", true, "client",
            cmdline::oneof<std::string>("server", "client"));
    parser.add<std::string>("ip", 0, "server ip", true);
    parser.add<short>("port", 0, "server port", true, 2140,
            cmdline::range(1, 65535));
    parser.add<std::string>("user", 'u', "client user name", false);
    parser.add("daemon", 'd', "server run as a daemon");


    /* set desc */
    std::stringstream ss;
    ss << "CR is a simple chatting room program by mzy " << std::endl
        << "CR could run as server or client " << std::endl
        << "  run as server, you should specify the server ip, port " << std::endl
        << "  run as client, you should specify the server ip, port and user name " << std::endl
        << "  client run with a CDK windows, you may input /h for help info ";
    
    
    parser.SetDesc(ss.str());

    parser.parse_check(argc, argv);

    role = parser.get<std::string>("role");
    if (role == "client") {
        isServer = false;
    } else if (role == "server") {
        isServer = true;
    } else {
        std::cerr << "invalid argument, --role" << std::endl;
        exit(1);
    }

    port = parser.get<short>("port");

    /* check ip okay */
    ip = parser.get<std::string>("ip");
    if (InvalidIP(ip)) {
        std::cerr << "invalid ip: " << ip;
        exit(1);
    }

    user = parser.get<std::string>("user");
    if (!isServer && user.empty()) {
        std::cerr << "user must be specified if run as a client!";
        exit(1);
    }
    
        
    if (isServer) {
        isDaemon = parser.exist("daemon");
        if (isDaemon) {
            daemon(0, 0);
        }

        g_logger = new ThreadLogger("CRServer");
        g_logger->Start();
        err = RunAsServer(ip, port);
    } else {
        g_logger = new ThreadLogger("CRClient");
        g_logger->Start();
        err = RunAsClient(ip, port, user);
    }

    g_logger->Stop();
    return err;
}


