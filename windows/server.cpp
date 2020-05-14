#include <Ws2tcpip.h>
#include <stdio.h>
#include <winsock2.h>

#define BUFFERSIZE 100

// struct sockaddr_in {
//     short int        sin_family; /* AF_INT(使用IPv4) */
//     unsigned short int sin_port;    /* Port(埠號) */
//     struct in_addr       sin_addr;   /* IP位址 */
// };
// sturct in_addr {
//     unsigned long int s_addr;
// };

int main(int argc, char *argv[])
{
    SOCKET server_sockfd, client_sockfd;
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

#ifdef IPV4
    server_address.sin_family = AF_INET;  // AF_INET(使用IPv4)
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);  //
    // 設定IP位址
    server_address.sin_port = PORT;  //設定埠號
#endif

#ifdef IPV6
    server_address.sin6_family = AF_INET6;
    server_address.sin6_addr.s6_addr = SERVER_IP;
    server_address.sin6_port = PORT;
#endif

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

    int rVal;
    char buf[BUFFERSIZE + 1];
    while ((rVal = recv(client_sockfd, buf, BUFFERSIZE, 0)) > 0) {
        buf[rVal] = 0x00;
        printf("%s", buf);
    }

    printf("END....");
    closesocket(client_sockfd);
}
