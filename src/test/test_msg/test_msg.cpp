
#include <iostream>
#include "include/Msg.hpp"
#include "include/common.h"
#include <stdint.h>
#include "include/Log.hpp"

ThreadLogger *g_logger = NULL;
int main()
{
    unsigned short a = 12;
    int b = 3000;
    uint32_t c = 3333;
    uint64_t d = 1112222222211111;
    std::string e = "aaaa";
    std::string f;

    Msg msg;
    msg << a << b << c << d << e;

    Msg *t = msg.Dup();
    (*t) >> a >> b >> c >> d >> e;


    uint64_t temp = htonll(d);
    uint64_t temp2 = ntohll(temp);


    std::cout << a <<" " << b << " " << c <<  " " << d  << " " << e <<std::endl;
    std:: cout << temp << " " << temp2 << std::endl; 
    delete t;

    debug_log("HHAHAH");


}

