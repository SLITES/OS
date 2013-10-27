#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main()
{
    int server_fd, server_ln;
    struct sockaddr_in server_addres;
    server_fd = socket(AF_INET,SOCK_STREAM,0);
    server_addres.sin_family = AF_INET;
    server_addres.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addres.sin_port = 1234;
    server_ln = sizeof(server_addres);
    bind(server_fd, (struct sockaddr *) &server_addres, server_ln);
    return 0;
}
