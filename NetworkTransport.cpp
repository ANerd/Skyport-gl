
#include "NetworkTransport.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include "core/Debug.h"
#include "core/Error.h"

using namespace anengine;

void NetworkTransport::Connect(std::string hostname, std::string port)
{
    if(myFD != -1)
        throw Error(Error::InvalidState, "Connecting connected socket");

    addrinfo hints;
    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    addrinfo *ainfo;
    int r = getaddrinfo(hostname.c_str(), port.c_str(), &hints, &ainfo);
    if(r != 0)
    {
        throw Error(Error::InvalidValue, "Failed to get addr: "
                +std::string(gai_strerror(r)));
    }

    for(addrinfo *cur = ainfo; cur != NULL; cur = cur->ai_next)
    {
        myFD = socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol);
        if(myFD == -1)
        {
            perror("Failed to create socket");
            errno = 0;
            continue;
        }
        r = connect(myFD, cur->ai_addr, cur->ai_addrlen);
        if(r == 0)
            break;

        char hostname[255];
        char service[32];
        std::string endpoint;
        if(getnameinfo(cur->ai_addr, cur->ai_addrlen, hostname, 255, 
                    service, 32, NI_NUMERICSERV | NI_NUMERICHOST) == 0)
        {
            endpoint += hostname;
            endpoint += ':';
            endpoint += service;
        }
        perror(("Failed to connect socket:\""+endpoint+"\"").c_str());
        errno = 0;
        close(myFD);
        myFD = -1;
    }

    freeaddrinfo(ainfo);
    if(myFD == -1)
        throw Error(Error::InvalidValue, "Failed to connect to all sockets");
}

void NetworkTransport::Disconnect()
{
    close(myFD);
    myFD = -1;
}
void NetworkTransport::Send(std::string data)
{
    Bug(myFD == -1, "Sending on closed socket");
    if(write(myFD, data.c_str(), data.length()) == -1)
    {
        perror("Failed to send data");
        throw Error(Error::InvalidState, "Socket failed");
    }
}
std::string NetworkTransport::Recv(char separator)
{
    Bug(myFD == -1, "Reciving on closed socket");
    std::string recv;
    char c = 0;
    while(c != '\n')
    {
        if(read(myFD, &c, 1) == -1)
        {
            if(errno == EINTR)
            {
                Debug("N:Interrupted, exit.");
                recv.clear();
                break;
            }
            else
            {
                perror("Failed to recive data");
                throw Error(Error::InvalidState, "Socket failed");
            }
        }
        recv += c;
    }
    return recv;
}
