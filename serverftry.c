#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/un.h>
#define SOCK_PATH "chhavik_chat_system"
#define LOCK_PATH "locksys"
pthread_t client_threads[SOMAXCONN];
struct Client
{
	int clientNo;
	struct sockaddr_in clientname;
	socklen_t client_size;
	int clientSocketID;
	char username[1024];
};
int numberofClients=1;

struct Client clients[SOMAXCONN];
int splitString(char *bs,char arr[1024][1024])
{
	int i;
	int j=0;
	int ctr=0;
    for(i=0;i<=(strlen(bs));i++)
    {
        if(bs[i]==' '||bs[i]=='\0')
        {
            arr[ctr][j]='\0';
            ctr++;  
            j=0;    
        }
        else
        {
            arr[ctr][j]=bs[i];
            j++;
        }
    }
    return ctr;
}
void * clientTask(void * mainclient)
{
	struct Client* cl=(struct Client*) mainclient;
	while(1)
	{
		char msg[1024];
		int len;
		memset(&msg,'\0',1024);
		int readstatus=recv(cl->clientSocketID,msg,1024,0);
		if(readstatus==-1)
		{
			printf("Error in recieving message\n");
		}
		else
		{
			char arr[1024][1024];
			int t=splitString(msg,arr);
			char finalmsg[1024];
			strncpy(finalmsg,msg+2,strlen(msg)-1);
			char sendingclient[1024];
			sprintf(sendingclient,"%d",cl->clientNo);
			strcat(sendingclient,": ");
			strcat(sendingclient,cl->username);
			strcat(finalmsg," -message from client: ");
			strcat(finalmsg,sendingclient);
			
			if(strcmp("A",arr[0])==0)
			{
				for(int k=0;k<numberofClients;k++)
				{
					if(clients[k].clientSocketID!=cl->clientSocketID)
						sendto(clients[k].clientSocketID,(const char *)finalmsg,strlen(finalmsg),MSG_CONFIRM,(const struct sockaddr *) &clients[k].clientname,len);
					
				}
			}
			else if(strcmp("B",arr[0])==0)
			{
				printf("wrong input!\n");
			}
			
			else
			{
				int reciever=atoi(arr[0]);
				if(!(reciever>0 && reciever<10000))
				{
					printf("wrong input\n");
					char errmsg[1024];
					strcpy(errmsg,"wrong input");
					send(cl->clientSocketID,errmsg,1024,0);
				}
				if(clients[reciever-1].clientSocketID==cl->clientSocketID)
				{
					printf("wrong input\n");
					char errmsg[1024];
					strcpy(errmsg,"wrong input, cannot send a message to yourself");
					send(cl->clientSocketID,errmsg,1024,0);
				}
				if(clients[reciever-1].clientSocketID!=cl->clientSocketID)
					send(clients[reciever-1].clientSocketID,finalmsg,1024,0);
			
			}

		}
	}
}

int main(int argc, char *argv[])
{
	
	/*Socket creation*/
	int socketno;
	struct sockaddr_un sockname,remote;
	int len;
	socketno = socket(AF_UNIX, SOCK_STREAM, 0);
	if(socketno==-1)
	{
		printf("Cannot create socket");
		return -1;
	}
	/*Socket Binding*/
	/*sockname.sin_family=AF_INET;
	sockname.sin_port=htons(8088);
	sockname.sin_addr.s_addr = htons(INADDR_ANY);//INADDR_ANY
	int bindval=bind(socketno,(struct sockaddr *) &sockname , sizeof(sockname));*/
	sockname.sun_family = AF_UNIX;
	strcpy(sockname.sun_path,SOCK_PATH);
	len = strlen(sockname.sun_path)+sizeof(sockname.sun_family);

	int lockfd=open(LOCK_PATH, O_RDONLY|O_CREAT,0600);
	if(lockfd==-1)
	{
		printf("error in lock file...exiting");
		return -1;
	}
	int rt=flock(lockfd,LOCK_EX | LOCK_NB);
	if(rt!=0)
	{
		printf("socket already in use by another process....exiting");
		return -1;
	}
	unlink(SOCK_PATH);
	int bindval=bind(socketno,(struct sockaddr *) &sockname , len);
	if(bindval<0)
	{
		printf("Problem with binding....exiting");
		return -1;
	}
	int lps=listen(socketno,SOMAXCONN);
	if(lps==-1)
	{
		printf("Problem in listening....exiting");
		return -1;
	}
	printf("Server is running....\nWaiting for clients on port chhavik_chat_system\n");
	while(1)
	{

		int cl=accept(socketno,(struct sockaddr *)&clients[numberofClients-1].clientname,&clients[numberofClients-1].client_size);
		if(cl<0)
		{
			printf("Connection failed");
		}
		else
		{
			printf("Client %d accepted\n",numberofClients);
			clients[numberofClients-1].clientSocketID=cl;
			char usrname[1024];
			memset(&usrname,'\0',1024);
			int rs=recv(cl,usrname,1024,0);
			strcpy(clients[numberofClients-1].username,usrname);
		}
		pthread_create(&client_threads[numberofClients-1],NULL,clientTask,(void *) &clients[numberofClients-1]);
		clients[numberofClients-1].clientNo=numberofClients;
		numberofClients=numberofClients+1;

	}
	close(socketno);

}
