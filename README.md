Literally just runs socket calls with proper error checking. renoves possibility of laze creating security risks. will add linux and all socket call functionality soon

------------------     With sockwrap     ------------------ 
```
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <C:\2025PROJECTS\c\sockwrap.h>

int main(){
    char buffer[1000];
    
    //  Windows setup
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        fprintf(stderr, "WSAStartup sucks dick: %d", WSAGetLastError());
    return 1;
    }
    

    // Init server and addr
    struct socketman server;
    struct sockaddr_in addr;
    server.sockfd = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8000);
    addr.sin_addr.s_addr = INADDR_ANY;
    server.addr = (struct sockaddr*)&addr;
    server.addrlen = (sizeof(addr));
    socklen_t addrlen = sizeof(addr);

    // Bind server to sys
    safe_exec(&server, BIND_OP, NULL, 0);
    safe_exec(&server, LISTEN_OP, NULL, 0);
    printf("Socket established\n\tAddr:\t%s\n\tPort:\t%d\n",
        inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

    // Recv msgs
    struct sockaddr_in client_addr;
    struct socketman client;
    client.addrlen = sizeof(client_addr);
    client.sockfd = safe_exec(&server, ACCEPT_OP, NULL, 0); 
    safe_exec(&client, RECV_OP, buffer, sizeof(buffer));

    printf("Client connected:\n\tAddr:\t%s\n\tPort:\t%d\nMessage:\n\t%s\n",
        inet_ntoa(client_addr.sin_addr),
        ntohs(client_addr.sin_port),
        buffer);

    closesocket(client.sockfd);
    WSACleanup();
    return 0;
    
}
```

------------------     Without      ------------------ 
```
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    char buffer[1000];

    // Windows socket setup
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        fprintf(stderr, "WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }

    // Create server socket
    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
        fprintf(stderr, "Socket creation failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Bind address and port
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8000);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        fprintf(stderr, "Bind failed: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    // Listen on socket
    if (listen(server_fd, SOMAXCONN) == SOCKET_ERROR) {
        fprintf(stderr, "Listen failed: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    printf("Socket established\n\tAddr:\t%s\n\tPort:\t%d\n",
           inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

    // Accept client connection
    struct sockaddr_in client_addr;
    int client_addrlen = sizeof(client_addr);

    SOCKET client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addrlen);
    if (client_fd == INVALID_SOCKET) {
        fprintf(stderr, "Accept failed: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    // Receive message from client
    int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received == SOCKET_ERROR) {
        fprintf(stderr, "Receive failed: %d\n", WSAGetLastError());
        closesocket(client_fd);
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    buffer[bytes_received] = '\0'; // Null-terminate for safety

    printf("Client connected:\n\tAddr:\t%s\n\tPort:\t%d\nMessage:\n\t%s\n",
           inet_ntoa(client_addr.sin_addr),
           ntohs(client_addr.sin_port),
           buffer);

    closesocket(client_fd);
    closesocket(server_fd);
    WSACleanup();
    return 0;
}
```
