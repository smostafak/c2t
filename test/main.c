/* Created by Anjuta version 1.2.3 */
/*	This file will not be overwritten */

#include <stdio.h>
#include "iso8583.h"
#include "convert.h"
#include "iso8583_std.h"
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h>


int main()
{
	int mssock, membersoc;
	struct sockaddr_in address, client_add;
	int soklen;
	char *buf;
	soklen = sizeof(address);
    if ((mssock = socket(AF_INET,SOCK_STREAM,0)) < 0) {
        perror("server: socket");
        exit(1);
    }
	/* type of socket created in socket() */
	address.sin_family = AF_INET;
  	address.sin_addr.s_addr = INADDR_ANY;
	/* 7000 is the port to use for connections */
  	address.sin_port = htons(7000);
	/* bind the socket to the port specified above */
 	if (bind(mssock,(struct sockaddr *)&address,sizeof(address)) < 0)
	{
		perror("server: socket");
		exit(1);
	}
	if (listen(mssock, 3) < 0)
	{
		perror("server: socket");
		exit(1);
	}
	for (;;)
	{
		if ((membersoc = accept(mssock, (struct sockaddr_in *)&client_add, &soklen)) < 0) {
        perror("server: accept");
        exit(1);
		}
		
//			printf("Dava ham fork 1");
			buf = malloc(1024);
			FILE *fp;
//			printf("Dava ham fork 2");
//			
			if (recv(membersoc, buf, 1024, 0))
			{
				isomsg m;
				int index;
				char *xmlbuf, isobuf;
				//printf("Dava ham fork");
				printf("\n%s", buf);
				buf = buf + 4;
				printf("\n%s", buf);
				init_message(&m, BMP_HEXA);
				unpack_message(&m, iso87, buf);
/*				for (index = 0; index < 129; index++)
				{
					if (m.fld[index] != NULL)
						printf("Truong thu %d: %s\n", index, m.fld[index]);
				}*/
				fp = fopen("dump_msg.log", "w");
				dump_message(fp, &m, 0);
				close(fp);
				free_message(&m);
				xmlbuf = iso_to_xml(buf, iso87, BMP_HEXA);
/*				isobuf = xml_to_iso(xmlbuf, iso87, BMP_HEXA);
				init_message(&m, BMP_HEXA);
				unpack_message(&m, iso87, isobuf);
				fp = fopen("xml.log", "wt");
				dump_message(fp, &m, FMT_XML);*/
				//close(fp);
				if (xmlbuf != NULL)
				{
					//printf("\n%s", xmlbuf);
					free(xmlbuf);
				}
				else
				{
					printf("Error during convert message");
					//exit(1);
				}
			}
		
		close(membersoc);
	
	}
	return (0);
}
