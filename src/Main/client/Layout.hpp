

#ifndef __LAYOUT_HPP__
#define __LAYOUT_HPP__

#include <cdk.h>
#include "Client.hpp"

class Layout {
public:
    Layout(CRClient*client):mClient(client)
    {}
    ~Layout()
    {}

public:
    int Start();

public:
    CDKSCREEN *mScreen;
    CDKENTRY *mInput;
    WINDOW *mWindow;
    CDKSWINDOW *mDisplay;
    CDKSWINDOW *mList;
    CRClient *mClient;
};

#endif


