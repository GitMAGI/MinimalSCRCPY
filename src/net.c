#include "net.h"

#include <stdio.h>

#include "custom/log.h"

#ifdef __WINDOWS__
  typedef int socklen_t;
#else
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
# define SOCKET_ERROR -1
  typedef struct sockaddr_in SOCKADDR_IN;
  typedef struct sockaddr SOCKADDR;
  typedef struct in_addr IN_ADDR;
#endif

socket_t
net_connect(uint32_t addr, uint16_t port) {
    socket_t sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        //perror("socket");
        errorLog("Socket not valid");
        return INVALID_SOCKET;
    }

    SOCKADDR_IN sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(addr);
    sin.sin_port = htons(port);

    if (connect(sock, (SOCKADDR *) &sin, sizeof(sin)) == SOCKET_ERROR) {
        //perror("connect");
        errorLog("Connection failed");
        return INVALID_SOCKET;
    }

    return sock;
}

socket_t
net_listen(uint32_t addr, uint16_t port, int backlog) {
    socket_t sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        //perror("socket");
        errorLog("Socket not valid");
        return INVALID_SOCKET;
    }

    int reuse = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const void *) &reuse,
                   sizeof(reuse)) == -1) {
        //perror("setsockopt(SO_REUSEADDR)");
        errorLog("Socket Socket Option failure");
    }

    SOCKADDR_IN sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(addr); // htonl() harmless on INADDR_ANY
    sin.sin_port = htons(port);

    if (bind(sock, (SOCKADDR *) &sin, sizeof(sin)) == SOCKET_ERROR) {
        //perror("bind");
        errorLog("Socket Binding failed");
        return INVALID_SOCKET;
    }

    if (listen(sock, backlog) == SOCKET_ERROR) {
        //perror("listen");
        errorLog("Socket Listening failed");
        return INVALID_SOCKET;
    }

    //Log listening on port and ip
    struct sockaddr_in serv_addr_;
    socklen_t serv_addr_len_ = sizeof(struct sockaddr_in);
    getsockname(sock, (struct sockaddr *)&serv_addr_, &serv_addr_len_);
    char *serv_ip = inet_ntoa(serv_addr_.sin_addr);
    uint16_t serv_port = htons (serv_addr_.sin_port);
    infoLog("Host listening on %s:%d", serv_ip, serv_port);

    return sock;
}

socket_t
net_accept(socket_t server_socket) {
    SOCKADDR_IN csin;
    socklen_t sinsize = sizeof(csin);
    SOCKET socket = accept(server_socket, (SOCKADDR *) &csin, &sinsize);
    if(socket == INVALID_SOCKET){
        criticalLog("Accepting Connection failed");        
        return socket;
    }

    //Log Accepted remote ip and port
    struct sockaddr_in clnt_addr_;
    socklen_t clnt_addr_len_ = sizeof(struct sockaddr_in);
    getsockname(socket, (struct sockaddr *)&clnt_addr_, &clnt_addr_len_);
    char *clnt_ip = inet_ntoa(clnt_addr_.sin_addr);
    uint16_t clnt_port = htons (clnt_addr_.sin_port);
    infoLog("Accepted %s remote host on port %d", clnt_ip, clnt_port);

    return socket;
}

ssize_t
net_recv(socket_t socket, void *buf, size_t len) {
    return recv(socket, buf, len, 0);
}

ssize_t
net_recv_all(socket_t socket, void *buf, size_t len) {
    return recv(socket, buf, len, MSG_WAITALL);
}

ssize_t
net_send(socket_t socket, const void *buf, size_t len) {
    return send(socket, buf, len, 0);
}

ssize_t
net_send_all(socket_t socket, const void *buf, size_t len) {
    ssize_t w = 0;
    while (len > 0) {
        w = send(socket, buf, len, 0);
        if (w == -1) {
            return -1;
        }
        len -= w;
        buf = (char *) buf + w;
    }
    return w;
}

bool
net_shutdown(socket_t socket, int how) {
    return !shutdown(socket, how);
}