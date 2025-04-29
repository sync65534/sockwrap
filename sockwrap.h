#ifndef SOCKETMAN_H
#define SOCKETMAN_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    BIND_OP,
    CONNECT_OP,
    LISTEN_OP,
    ACCEPT_OP,
    RECV_OP,
    SEND_OP,
    NUM_OPS
} SocketOperation;

struct socketman {
    int sockfd;
    struct sockaddr* addr;
    int addrlen;
};

typedef int (*SocketFunc)(struct socketman*, void*, int); // Unified function signature

// --------- Safe Socket Operators -----------

int bind_handler(struct socketman* sck, void* buf, int bufsize) { 
    (void)buf; (void)bufsize; // Unused
    if (bind(sck->sockfd, sck->addr, sck->addrlen) < 0) {
        fprintf(stderr, "Error: bind: %d\n", WSAGetLastError());
        closesocket(sck->sockfd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    return 0;
}

int connect_handler(struct socketman* sck, void* buf, int bufsize) {
    (void)buf; (void)bufsize;
    if (connect(sck->sockfd, sck->addr, sck->addrlen) < 0) {
        fprintf(stderr, "Error: connect: %d\n", WSAGetLastError());
        closesocket(sck->sockfd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    return 0;
}

int listen_handler(struct socketman* sck, void* buf, int bufsize) {
    (void)buf;
    int backlog = bufsize;
    if (listen(sck->sockfd, backlog) < 0) {
        fprintf(stderr, "Error: listen: %d\n", WSAGetLastError());
        closesocket(sck->sockfd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    return 0;
}

int accept_handler(struct socketman* sck, void* buf, int bufsize) {
    (void)buf; (void)bufsize;
    int clientfd = accept(sck->sockfd, NULL, NULL);
    if (clientfd == INVALID_SOCKET) {
        fprintf(stderr, "Error: accept: %d\n", WSAGetLastError());
        closesocket(sck->sockfd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    return clientfd;
}

int recv_handler(struct socketman* sck, void* buf, int bufsize) {
    ;
    if (recv(sck->sockfd, (char*)buf, bufsize, 0) <= 0) {
        fprintf(stderr, "Error: recv: %d\n", WSAGetLastError());
        closesocket(sck->sockfd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    return 0;
}

int send_handler(struct socketman* sck, void* buf, int bufsize) {
    if (send(sck->sockfd, (char*)buf, bufsize, 0) <= 0) {
        fprintf(stderr, "Error: send: %d\n", WSAGetLastError());
        closesocket(sck->sockfd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    return 0;
}

// ------------ Syscall table -------------------

SocketFunc socket_syscalls[NUM_OPS] = {
    bind_handler,
    connect_handler,
    listen_handler,
    accept_handler,
    recv_handler,
    send_handler
};

// ------------ Main Execution function ------------

int safe_exec(struct socketman* sck, SocketOperation op, void* buf, int bufsize) {
    if (op >= NUM_OPS) {
        fprintf(stderr, "Invalid socket operation.\n");
        closesocket(sck->sockfd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    return socket_syscalls[op](sck, buf, bufsize);
}

#endif // SOCKETMAN_H
