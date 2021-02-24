#include "Server.h"
#include "Util.h"
#include "HttpData.h"

//#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

 const int Server::kMaxFds = 100000;
Server::Server(EventLoop *loop, int numThreads, int port):
    loop_(loop), 
    numThreads_(numThreads),
    port_(port),
    started_(false),
    listenFd_(socket_bind_listen(port_)),
    listenChannel_(new Channel(loop_, listenFd_)),
    eventLoopThreadPool_(new EventLoopThreadPool(loop_, numThreads_))
{
    handle_for_sigpipe(); // figrue out this;
    if(setSocketNonBlocking(listenFd_) < 0)
    {
        perror("set socket non block faild");
        abort();
    }
}

void Server::start()
{
    eventLoopThreadPool_->start();
    listenChannel_->setEvent(EPOLLIN | EPOLLET);
    listenChannel_->setReadHandler(std::bind(&Server::handleConnctSocket, this));
    listenChannel_->setConnHandler(std::bind(&Server::handleListenSocket, this));
    loop_->updateChannel(listenChannel_);
    started_ = true;
}

 void Server::handleConnctSocket()
 {
     struct sockaddr_in clientAddress;
     memset(&clientAddress, 0,  sizeof(clientAddress));
     socklen_t clientAddressLength = sizeof(clientAddress);
     int connFd = 0;
     while( (connFd = accept(listenFd_, (struct sockaddr*)&clientAddress, &clientAddressLength)) > 0)
     {
        EventLoop *loop = eventLoopThreadPool_->getNextLoop();
        printf("New connection from %d : %d ", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port) );
        if(connFd >= kMaxFds)
        {
            close(connFd);
            continue;
        }
        if(setSocketNonBlocking(connFd) < 0)
        {
            perror("Set non block failed!");
            return;
        }
        setSocketNodelay(connFd);
        std::shared_ptr<HttpData> httpRequest(new HttpData(loop, connFd));
        httpRequest->getChannel()->setHolder(httpRequest);
        loop->queueInLoop(std::bind(&HttpData::newEvent, httpRequest)); 
     }
     listenChannel_->setEvent(EPOLLIN | EPOLLET);
     
 }
 void Server::handleListenSocket()
 {
     loop_->updateChannel(listenChannel_);
 }