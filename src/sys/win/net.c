#include "net.h"

#include "custom/log.h"

#include <ws2tcpip.h>

bool
net_init(void) {
    WSADATA wsa;
    int res = WSAStartup(MAKEWORD(2, 2), &wsa) < 0;
    if (res < 0) {
        criticalLog("WSAStartup failed with error %d", res);
        return false;
    }
    return true;
}

void
net_cleanup(void) {
    WSACleanup();
}

bool
net_close(socket_t socket) {
    if(socket == INVALID_SOCKET){
        criticalLog("Unable to close an invalid socket");
        return false;
    }

    struct sockaddr_in clnt_addr_;
    socklen_t clnt_addr_len_ = sizeof(struct sockaddr_in);
    getsockname(socket, (struct sockaddr *)&clnt_addr_, &clnt_addr_len_);
    char *clnt_ip = inet_ntoa(clnt_addr_.sin_addr);
    uint16_t clnt_port = htons (clnt_addr_.sin_port);    

    int status = closesocket(socket);

    if(status < 0)
        criticalLog("Disconnection failed. Remote host %s on port %d", clnt_ip, clnt_port);
    else
        infoLog("Disconnected remote host %s on port %d", clnt_ip, clnt_port);

    return !status;
}
