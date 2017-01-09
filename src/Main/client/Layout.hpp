

#ifndef __LAYOUT_HPP__
#define __LAYOUT_HPP__

#include <cdk.h>
#include <vector>
#include <string>

class CRClient;

class Layout {
public:
    Layout(CRClient*client):mClient(client)
    {}
    ~Layout()
    {}

public:
    int Start();

    void UpdateUserListWithStringVec(const std::vector<std::string>& vec);
    void ReceiveMessage(const std::string& from , const std::string& words, bool isSelf);

public:
    CDKSCREEN *mScreen;
    CDKENTRY *mInput;
    WINDOW *mWindow;
    CDKSWINDOW *mDisplay;
    CDKSWINDOW *mList;
    CRClient *mClient;
};

#endif


