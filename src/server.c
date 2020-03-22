#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#define SERVER_PORT "64444"
#define SERVER_BACKLOG 1

int start_server()
{
    // get address info to listen to
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *server_ai;
    int error;
    if ((error = getaddrinfo(NULL, SERVER_PORT, &hints, &server_ai)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error));
        return -1;
    }

    // get the socket
    int socket_fd = socket(server_ai->ai_family,
                           server_ai->ai_socktype,
                           server_ai->ai_protocol);
    if (-1 == socket_fd)
    {
        fprintf(stderr, "socket: %s\n", strerror(errno));
        return -1;
    }

    int yes = 1;
    if (-1 == setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes,
                         sizeof(int)))
    {
        fprintf(stderr, "setsockopt: %s\n", strerror(errno));
        return -1;
    }

    // bind
    if (-1 == bind(socket_fd,
                   server_ai->ai_addr,
                   server_ai->ai_addrlen))
    {
        fprintf(stderr, "bind: %s\n", strerror(errno));
        return -1;
    }

    // listen
    if (-1 == listen(socket_fd, SERVER_BACKLOG))
    {
        fprintf(stderr, "listen: %s\n", strerror(errno));
        return -1;
    }

    return socket_fd;
}