
#include <cdk.h>
#include <stdlib.h>
#include "Layout.hpp"

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
        memset(temp, 0, 256);
        input = activateCDKEntry(mInput, 0);

        /* 这里我们最好能够截取到tab键 然后根据场景来自动补全 
         * 参考buttonbox_ex 中的写法
         * */

        if (mInput->exitType == vNORMAL) {
            sprintf(temp, "input: %s", input);
            addCDKSwindow(mDisplay, input, BOTTOM);
        } else {
            sprintf(temp, "error exit: %s", input);
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



