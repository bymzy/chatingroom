
#include <iostream>
#include "include/Log.hpp"
#include "include/ThreadLogger.hpp"

ThreadLogger *g_logger = NULL;

int main()
{
    g_logger = new ThreadLogger("Test");
    g_logger->Start();

    for (int i = 0; i < 1000 ; ++i) {
        debug_log(""<< i);
        warn_log(""<< i);
        info_log(""<< i);
        trace_log(""<< i);
        error_log(""<< i);
    }

    sleep(100);

}
