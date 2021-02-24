#include "EventLoop.h"
//#include "Channel.h"
#include "Server.h"
//#include <sys/timerfd.h>
//#include <cstring>


int main(int argc, char *argv[])
{    
    EventLoop mainLoop;
    int threadNum = 4, port = 9999;
    Server myHTTPServer(&mainLoop,  threadNum, port);
    myHTTPServer.start();
    mainLoop.loop();
    return 0;

}
