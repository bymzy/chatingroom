
#include <cdk.h>
#include <stdlib.h>
#include <stdint.h>
#include "Layout.hpp"
#include "Client.hpp"

static int entryCB(EObjectType cdktype GCC_UNUSED,
        void *object GCC_UNUSED,
        void *clientData, chtype key)
{
    char data[256];
    char *value;
    CDKENTRY *entry = (CDKENTRY*)clientData;
    value = getCDKEntryValue(entry);
    sprintf(data, "%s !!!!!!!!!!!!", value);
    setCDKEntryValue(entry, data);
    activateCDKEntry(entry, 0);
}

int 
Layout::Start()
{
    char temp[256];
    char *input = NULL;
    bool exit = false;
    bool logon = true;

    mWindow = initscr();
    mScreen = initCDKScreen(mWindow);
    initCDKColor();

    int displayWidth = COLS * 3 / 4;

    mDisplay = newCDKSwindow(mScreen,
            0,
            0,
            LINES - 4,
            displayWidth,
            "",
            100,
            TRUE,
            FALSE);

    mList = newCDKSwindow(mScreen,
            displayWidth + 1,
            0,
            LINES - 4,
            COLS - displayWidth,
            "",
            100,
            TRUE,
            FALSE);

    mInput= newCDKEntry(mScreen,
            0,
            getbegy(mDisplay->win) + mDisplay->boxHeight,
            "", 
            "INPUT: ",
            A_NORMAL,
            ' ',
            vMIXED,
            COLS, 
            0,
            256,
            TRUE,
            FALSE);

    drawCDKSwindow(mDisplay, ObjOf (mDisplay)->box);
    drawCDKSwindow(mList, ObjOf (mList)->box);
    refreshCDKScreen(mScreen);
    bindCDKObject(vENTRY, mInput, KEY_TAB, entryCB, mInput);
    
    while(!exit) {
        if (logon) {
            mClient->SendLogon();
            logon = false;
        }

        memset(temp, 0, 256);
        input = activateCDKEntry(mInput, 0);

        if (input == NULL) {
            continue;
        }
        /* 这里我们最好能够截取到tab键 然后根据场景来自动补全 
         * 参考buttonbox_ex 中的写法
         * */

        if (mInput->exitType == vNORMAL) {
            sprintf(temp, "%s", input);
            //addCDKSwindow(mDisplay, temp, BOTTOM);
            mClient->HandleInput(input);
        } else {
            sprintf(temp, "error exit: %s", input);
            addCDKSwindow(mDisplay, temp, BOTTOM);
            exit = true;
        }
        setCDKEntryValue(mInput, "");
    }

    destroyCDKEntry(mInput);
    destroyCDKSwindow(mDisplay);
    destroyCDKSwindow(mList);
    destroyCDKScreen(mScreen);
    endCDK();
}

void
Layout::UpdateUserListWithStringVec(const std::vector<std::string>& vec)
{
    uint32_t i = 0;
    char temp[255];

    cleanCDKSwindow(mList);
    for (;i < vec.size(); ++i) {
        memset(temp, 255, 0);
        sprintf(temp, "%s", vec[i].c_str());
        addCDKSwindow(mList, temp, BOTTOM);
    }
}

void
Layout::ReceiveMessage(const std::string& from, const std::string& words, bool isSelf)
{
    std::string output;
    char temp[256];
    memset(temp, 0, 256);
    if (isSelf) {
        output = "<R></B/05>" + words + " :" + from;
    } else {
        output = "<L></05>" + from + ": "  + words;
    }
    sprintf(temp, "%s", output.c_str());
    addCDKSwindow(mDisplay, temp, BOTTOM);
}

void
Layout::DisplaySystemMessage(const std::string& info)
{
    std::string text= "</B/31>system info: " + info;
    addCDKSwindow(mDisplay, text.c_str(), BOTTOM);
}


