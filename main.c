#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void sendBuf(char message[], int client_fd)
{
    int total = 0;
    int len = sizeof(message);
    int n;
    while(total < len)
    {
        n = send(client_fd, (char *)&message, sizeof(message),0);
        if(n == -1)
            break;
        total += n;
    }
}

int main()
{
    char message[] = "HTTP/1.1 200 OK\r\nServer: YarServer/2009-09-09\r\nContent-Type: text/html\r\nContent-Length: 43\r\nConnection: close\r\n\r\n<html><body>сервер работает</body></html>\n";
    int server_fd, server_ln;
    int client_fd, client_ln;
    int bind_s, list_s;
    struct sockaddr_in server_addres;
    struct sockaddr_in client_addres;
    server_fd = socket(AF_INET,SOCK_STREAM,0);
    server_addres.sin_family = AF_INET;
    server_addres.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addres.sin_port = 1235;
    server_ln = sizeof(server_addres);
    bind_s = bind(server_fd, (struct sockaddr *) &server_addres, server_ln);
    list_s = listen(server_fd,5);
    printf("Bind %d, List %d",bind_s, list_s);
    while(1)
    {
        client_ln = sizeof(client_addres);
        client_fd = accept(server_fd, (struct sockaddr *) &client_addres, &client_ln);
        printf("Client FD %d",client_fd);
        sendBuf(message,client_fd);
    }
    return 0;
}
