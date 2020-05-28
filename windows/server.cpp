#include <Ws2tcpip.h>
#include <process.h>
#include <stdio.h>
#include <winsock2.h>
#include <iostream>

#define BUFFERSIZE 1000

SOCKET server_sockfd, client_sockfd;
void ClientMessage(void *p);
void recvFile(char *filepath);

int main()
{
    int server_len, client_len;

#ifdef IPV4
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
#endif
#ifdef IPV6
    struct sockaddr_in6 server_address;
    struct sockaddr_in6 client_address;
#endif

    // 註冊 Winsock DLL
    WSADATA wsadata;
    if (WSAStartup(0x101, (LPWSADATA) &wsadata) != 0) {
        printf("Winsock Error\n");
        exit(1);
    }

    // 產生 server socket
    // AF_INET(使用IPv4); SOCK_STREAM; 0(使用預設通訊協定，即TCP)
#ifdef IPV4
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
#endif

#ifdef IPV6
    server_sockfd = socket(AF_INET6, SOCK_STREAM, 0);
#endif


    if (server_sockfd == SOCKET_ERROR) {
        printf("Socket Error\n");
        exit(1);
    }

    // struct sockaddr_in {
    //     short int               sin_family; /* AF_INT(使用IPv4) */
    //     unsigned short int sin_port;    /* Port(埠號) */
    //     struct in_addr       sin_addr;   /* IP位址 */
    // };
    // sturct in_addr {
    //     unsigned long int s_addr;
    // };
    server_address.sin_family = AF_INET;  // AF_INT(使用IPv4)
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);  // 設定IP位址
    server_address.sin_port = PORT;                         //設定埠號
    server_len = sizeof(server_address);

    if (bind(server_sockfd, (struct sockaddr *) &server_address, server_len) <
        0) {
        printf("Bind Error\n");
        exit(1);
    }

    if (listen(server_sockfd, 5) < 0) {
        printf("Listen Error\n");
        exit(1);
    }

    printf("Server waiting...\n");
    client_len = sizeof(client_address);
    client_sockfd =
        accept(server_sockfd, (struct sockaddr *) &client_address, &client_len);
    if (client_sockfd == SOCKET_ERROR) {
        printf("Accept Error\n");
        exit(1);
    }
    printf("Client connect.\n");
    printf("......\n");
    printf("...\n");

    _beginthread(ClientMessage, 0, NULL);

    while (1) {
        char buf[BUFFERSIZE + 1];
        scanf("%s", buf);

        send(client_sockfd, buf, BUFFERSIZE, 0);
    }



    printf("END....");
    closesocket(client_sockfd);
}

void ClientMessage(void *p)
{
    int rVal;
    char buf[BUFFERSIZE + 1];
    while ((rVal = recv(client_sockfd, buf, BUFFERSIZE, 0)) > 0) {
        buf[rVal] = 0x00;
        if (strcmp(buf, "#screenshot") == 0) {
            char filename[40] = SCREENSHOTPATH;
            recvFile(filename);

        } else {
            printf("[Client] >> %s\n", buf);
        }
    }
}

void recvFile(char *filepath)
{
    char imgBuf[FILEBLOCKSIZE];

    int ret, byterecv;
    int err;
    FILE *ptrFile;
    ptrFile = fopen(filepath, "wb");

    byterecv = 0;

    do {
        ret = recv(client_sockfd, imgBuf, FILEBLOCKSIZE, 0);
        if (strcmp(imgBuf, "#END") == 0) {
            break;
        }
        byterecv += ret;
        fwrite(imgBuf, sizeof(char), ret, ptrFile);
    } while (ret > 0);

    fclose(ptrFile);

    printf("[ Stored at: %s ]\n", filepath);
    printf("[ %d byte received. ]\n", byterecv);
}
