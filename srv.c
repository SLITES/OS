#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/shm.h>
#include <signal.h>
#define COUNT_PROC 5
#define HOME "/home/slite/Документы/Учеба/Server/doc"

struct shared_attr
{
	int pid[COUNT_PROC];
	int used[COUNT_PROC];
} 

int shmid;
void *shared_mem;
shared_attr *proc_attr;

void sendBuf(char *message,int size, int client_fd)
{
    int total = 0;
    int n=0;
    while(total < size)
    {
        n = send(client_fd,(message+n), size-n,0);        
		if(n == -1)
            break;
        total += n;
    }
}

void AddShm()
{
	shmid = shmget((key_t)1234,sizeof(shared_attr),0666 | IPC_CREAT);
    if (shmid == -1)
    {
        fprintf(stderr,"Error shmget\n");
        exit(EXIT_FAILURE);
    }
	shared_mem = shmat(shmid,(void *)0,0);
    if (shared_mem == (void *)-1)
    {
        fprintf(stderr,"Error shmat\n");
        exit(EXIT_FAILURE);
    }
    proc_attr = (shared_attr *)shared_mem;
}

void close_srv()
{
	int i;
	for (i=0;i<COUNT_PROC;i++)
	{
		kill(proc_attr->pid[i], SIGKILL);
	}
	if(shmdt(shared_memory) == -1)
	{
		fprintf(stderr,"Error shmdt\n");
		exit(EXIT_FAILURE);
	}

	if(shmctl(shmid,IPC_RMID,0) == -1)
	{
		fprintf(stderr,"Error shmctl(IPC_RMID)\n");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}

#define TRACE printf("%s; %d\n", __FILE__, __LINE__);
int main()
{
    printf("Server wait\n");
    char header[] = "HTTP/1.1 200 OK\r\n\n";    
	int server_fd, server_ln;
    int client_fd, client_ln;
    int bind_s, list_s;
	int i, child_pid;
	struct sockaddr_in server_addres;
    struct sockaddr_in client_addres;
	struct sigaction close_str;

    close_str.sa_handler = close_srv;
    sigemptyset(&close_str.sa_mask);
    close_str.sa_flags = 0;
	
	AddShm();

    server_fd = socket(AF_INET,SOCK_STREAM,0);
    server_addres.sin_family = AF_INET;
    server_addres.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addres.sin_port = htons(2235);
    server_ln = sizeof(server_addres);
    
	bind_s = bind(server_fd, (struct sockaddr *) &server_addres, server_ln);
	list_s = listen(server_fd,10);

    printf("Bind %d, List %d \n",bind_s, list_s);
	
	for (i=0;i<COUNT_PROC;i++)
	{	
		if ((child_pid=fork()) == 0)
		{
			while(1)
			{
				client_ln = sizeof(client_addres);
				client_fd = accept(server_fd, (struct sockaddr *) &client_addres, &client_ln);
				printf("Client connect with FD %d \n",client_fd);        
				//sendBuf(&header[0],sizeof(header),client_fd);
				proc_attr->used[i] = 1;
				process_client(client_fd);
				printf("Client disconect, FD %d \n",client_fd);
				close(client_fd);
			}				
		}
		else
		{
			proc_attr->used[i] = child_pid;			
		}		
	}    
	sigaction(SIGINT, &close_srv, 0);
    wait(NULL);
}

