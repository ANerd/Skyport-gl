#ifndef NETWORKTRANSPORT_H_
#define NETWORKTRANSPORT_H_

#include <string>

class NetworkTransport
{
    int myFD;
    public:
    NetworkTransport()
        : myFD(-1) { }
    void Connect(std::string hostname, std::string port);
    void Disconnect();
    void Send(std::string data);
    std::string Recv(char separator);
};

#endif
