#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "camera.h"

int connect_camera(const char *hostname)
{
    // get addr info
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int error;
    struct addrinfo *camera_ai;
    if ((error = getaddrinfo(hostname, "http", &hints, &camera_ai)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error));
        return -1;
    }

    // create socket
    int socket_fd = socket(camera_ai->ai_family,
                           camera_ai->ai_socktype,
                           camera_ai->ai_protocol);
    if (-1 == socket_fd)
    {
        fprintf(stderr, "socket: %s\n", strerror(errno));
        return -1;
    }

    // connect
    if (-1 == connect(socket_fd, camera_ai->ai_addr, camera_ai->ai_addrlen))
    {
        fprintf(stderr, "connect: %s\n", strerror(errno));
        return -1;
    }

    // handshake
    char *request = "GET /livestream/12 HTTP/1.1\r\nAuthorization: Basic YWRtaW46\r\n\r\n";
    if (-1 == send(socket_fd, request, strlen(request), 0))
    {
        fprintf(stderr, "send: %s\n", strerror(errno));
        return -1;
    }

    return socket_fd;
}
