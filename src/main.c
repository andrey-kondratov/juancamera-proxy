#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "camera.h"
#include "server.h"

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "usage: %s <hostname>\n", argv[0]);
        return 1;
    }

    // start server
    int server;
    if (-1 == (server = start_server()))
    {
        fprintf(stderr, "failed to start server\n");
        return 2;
    }

    // message loop
    while (1)
    {
        // accept a connection
        struct sockaddr_storage client_addr;
        socklen_t client_addr_size = sizeof client_addr;
        int client = accept(server, (struct sockaddr *)&client_addr,
                            &client_addr_size);
        if (-1 == client)
        {
            fprintf(stderr, "accept: %s\n", strerror(errno));
            continue;
        }

        char clientString[INET6_ADDRSTRLEN];
        inet_ntop(client_addr.ss_family,
                  get_in_addr((struct sockaddr *)&client_addr),
                  clientString, sizeof clientString);
        printf("got connection from %s\n", clientString);

        // connect to the camera
        int camera;
        const char const *hostname = argv[1];
        if (-1 == (camera = connect_camera(hostname)))
        {
            fprintf(stderr, "failed to connect to the camera\n");
            shutdown(client, SHUT_RDWR);
            continue;
        }

        // serve the data
        int error = 0, len = 16000;
        char buffer[len];
        while (error == 0)
        {
            int bytes_received = recv(camera, &buffer, len, 0);
            if (-1 == bytes_received)
            {
                fprintf(stderr, "recv: %s\n", strerror(bytes_received));
                error = -1;
                break;
            }

            int bytes_sent = send(client, &buffer, bytes_received, 0);
            if (-1 == bytes_sent)
            {
                fprintf(stderr, "send: %s\n", strerror(bytes_sent));
                error = -1;
                break;
            }
        }

        shutdown(client, SHUT_RDWR);
        shutdown(camera, SHUT_RDWR);

        printf("Dropped connection to %s\n", clientString);
    }

    shutdown(server, SHUT_RDWR);
    return 0;
}