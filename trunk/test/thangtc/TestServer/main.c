#include "define.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <string.h>

int (*p[2])() = {test1, test2};
int create_socket(int port)
{
	int mssock, membersoc;
	struct sockaddr_in address;
	int soklen;
	char *buf;
	soklen = sizeof(address);
    if ((mssock = socket(AF_INET,SOCK_STREAM,0)) < 0) {
        perror("server: socket");
        return -1;
    }
	address.sin_family = AF_INET;
  	address.sin_addr.s_addr = INADDR_ANY;
  	address.sin_port = htons(port);
 	if (bind(mssock,(struct sockaddr *)&address,sizeof(address)) < 0)
	{
		perror("server: socket");
		return -1;
	}
	if (listen(mssock, 5) < 0)
	{
		perror("server: socket");
		return -1;
	}
	return mssock;
}

void main(int argc, char *argv[])
{
	int index, numArr;
	int port, msSock, client_soc;
	struct sockaddr_in client_add;
	int soklen;
	char *buf;
	soklen = sizeof(client_add);	
	numArr = sizeof(Func_Arr)/sizeof(arrFunc);
	if(argc < 3)
	{
		printf("Khong day du tham so, gom:\nPort dung de connect\nTen ham dung de xu ly");
//		close(msSock);
		exit(1);
	}
	else
	{
		//Create socket to listen
/*		for (index = 0; index < strlen(argv[1]) - 1; index ++)
		{
			if (isdigit(argv[1][1]) == 0)
				printf("The port is not correct");
			exit(1);
		}*/
		port = atoi(argv[1]);
		if (port == NULL)
		{
			printf("Port error");
			exit(1);
		}
		msSock = create_socket(port);
//		msSock = -1;
		if (msSock == -1)
		{
			printf ("Can not open port");
			exit(1);
		}
		//Choose the function using to proccess
		for (index = 0; index < numArr - 1; index++)
		{
			if (strcmp(argv[2], Func_Arr[index].func_name) == 0)
				break;
		}
		index = Func_Arr[index].index;
		while (1)
		{
			printf("waiting for new connect...");
			client_soc = accept(msSock, (struct sockaddr_in *)&client_add, &soklen);
			if ( client_soc < 0) {
				perror("server: accept");
			
			}
		}	
	}
}
