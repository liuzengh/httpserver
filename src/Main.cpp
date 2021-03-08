#include "EventLoop.h"
#include "Server.h"
#include "log/logger.h"


int main(int argc, char *argv[])
{    
    EventLoop mainLoop;
    int threadNum = 4, port = 9999;
    Server httpServer(&mainLoop,  threadNum, port);
  //  LOG_INFO("I'm master thread");
    httpServer.start();
    mainLoop.loop();
    return 0;

}
