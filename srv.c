#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void sendBuf(char *message,int size, int client_fd)
{
    int total = 0;
    int n=0;
    printf("Client FD %d \n", client_fd);
    while(total < size)
    {
        n = send(client_fd,(message+n), size-n,0);
	printf("Result %d,  \n", n);        
	if(n == -1)
            break;
        total += n;
    }
}
#define TRACE printf("%s; %d\n", __FILE__, __LINE__);
int main()
{
    printf("Server wait\n");
    char header[] = "HTTP/1.1 200 OK\r\n\n";    
	int server_fd, server_ln;
    int client_fd, client_ln;
    int bind_s, list_s;
    struct sockaddr_in server_addres;
    struct sockaddr_in client_addres;
    server_fd = socket(AF_INET,SOCK_STREAM,0);
    server_addres.sin_family = AF_INET;
    server_addres.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addres.sin_port = htons(2235);
    server_ln = sizeof(server_addres);
    bind_s = bind(server_fd, (struct sockaddr *) &server_addres, server_ln);
	list_s = listen(server_fd,5);
TRACE
    printf("Bind %d, List %d \n",bind_s, list_s);
    while(1)
    {
		client_ln = sizeof(client_addres);
		client_fd = accept(server_fd, (struct sockaddr *) &client_addres, &client_ln);
		TRACE        
		sendBuf(&header[0],sizeof(header),client_fd);
    }
    return 0;
}

