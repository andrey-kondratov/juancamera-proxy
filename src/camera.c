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
    char *request1 = "GET /bubble/live?ch=0&stream=0 HTTP/1.1\r\n\r\n";
    if (-1 == send(socket_fd, request1, strlen(request1), 0))
    {
        fprintf(stderr, "handshake send #1: %s\n", strerror(errno));
        return -1;
    }

    char response1[1142];
    if (-1 == recv(socket_fd, &response1, sizeof response1, 0))
    {
        fprintf(stderr, "handshake recv #1: %s\n", strerror(errno));
        return -1;
    }

    char request2[] = {
        0xaa, 0, 0, 0, 0x35, 0, 0x0e, 0x16, 0xc2, 0x71, 0, 0, 0, 0x2c, 0, 0, 0, 0,
        0x61, 0x64, 0x6d, 0x69, 0x6e, // admin
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    if (-1 == send(socket_fd, &request2, sizeof request2, 0))
    {
        fprintf(stderr, "handshake send #2: %s\n", strerror(errno));
        return -1;
    }

    char response2[54];
    if (-1 == recv(socket_fd, response2, sizeof response2, 0))
    {
        fprintf(stderr, "handshake recv2: %s\n", strerror(errno));
        return -1;
    }

    char request3[] = {
        0xaa, 0, 0, 0, 0x15, 0x0a, 0x0e, 0x16, 0xc2, 0xdf,
        0, 0, 0, 0, 0, 0, 0, 0, 0x01, 0, 0, 0, 0, 0, 0, 0};
    if (-1 == send(socket_fd, &request3, sizeof request3, 0))
    {
        fprintf(stderr, "handshake send #3: %s\n", strerror(errno));
        return -1;
    }

    return socket_fd;
}
