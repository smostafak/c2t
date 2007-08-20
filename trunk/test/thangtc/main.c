/* Created by Anjuta version 1.2.3 */
/*	This file will not be overwritten */

#include <stdio.h>
#include "iso8583.h"
#include "convert.h"
#include "iso8583_std.h"
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <string.h>

void read_xml(char *filename)
{
	FILE *fp;
	int index, leng, isolen;
	isomsg m;
	char buf[256];
	char *xmlbuf, *isobuf;
	fp = fopen(filename, "rt");
	if (!fp)
	{
		perror(filename);
		exit(1);
	}
	/*if (fread(xmlbuf, 1, 1024, fp) == 0)
	{
		perror(filename);
		exit(1);
	}*/
	xmlbuf = malloc(2048);
	while (!feof(fp))
	{
		fgets(buf, 256, fp);
//		printf("%s\n", buf);
		strcat(xmlbuf, buf);
	}
	fclose(fp);
//	printf("%s", xmlbuf);
	leng = strlen(xmlbuf);
//	xmlbuf[leng - 1] = '>';
//	xmlbuf[leng] = '\n';
	printf("%s", xmlbuf);
	fp = fopen("err_xml.log", "wt");
	isolen = 0;
	isobuf = xml_to_iso(xmlbuf, iso87, BMP_HEXA, &isolen);
	if (isobuf == NULL)
	{
		printf("\nLoi khi parse");
		fclose(fp);
		exit(1);
	}
	
//	printf("%s\n", isobuf);
	init_message(&m, BMP_HEXA);
	unpack_message(&m, iso87, isobuf, isolen);
/*	for (index = 0; index < 129; index++)
	{
		if (m.fld[index] != NULL)
			printf("Truong %d: %s\n", index, m.fld[index]);
	}*/
	dump_message(fp, &m, 0);
	fclose(fp);
}
void receive_iso()
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

	address.sin_family = AF_INET;
  	address.sin_addr.s_addr = INADDR_ANY;

  	address.sin_port = htons(7000);

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


		if ((membersoc = accept(mssock, (struct sockaddr_in *)&client_add, &soklen)) < 0) {
        perror("server: accept");
        exit(1);
		
		}	
		
	
			buf = malloc(1024);
			FILE *fp;
			if (recv(membersoc, buf, 1024, 0))
			{
				
				isomsg m;
				int index;
				char *xmlbuf, *isobuf;
//				printf("\n%s", buf);
				buf = buf + 4;
				printf("\n%s", buf);
				init_message(&m, BMP_HEXA);
				if (unpack_message(&m, iso87, buf, 1024))
				{
					printf("\nError during unpack message");
					exit(1);
				}
				fp = fopen("dump_msg.log", "wt");
				dump_message(fp, &m, 0);
				fclose(fp);
				free_message(&m);
				xmlbuf = iso_to_xml(buf, iso87, BMP_HEXA, 1024);
				if (xmlbuf != NULL)
				{
					printf("Create xml dump message");
					fp = fopen("dump_msg.xml", "wt");
					if (fp != NULL)
					{
						fprintf(fp, "%s", xmlbuf);
						fclose(fp);
						//exit(1);
					}
					else
						printf("\n Can not create file");
					isobuf = xml_to_iso(xmlbuf, iso87, BMP_HEXA, 1024);
//					printf(xmlbuf);
					if (isobuf == NULL)
					{
						printf("\nKo unpack dc message dang iso");
						exit(1);
					}
					else	
					{
						fp = fopen("dump_msg_cvt.log", "wt");
						init_message(&m, BMP_HEXA);
						unpack_message(&m, iso87, buf, 1024);
						dump_message(fp, &m, 0);
						fclose(fp);
						
					}
					free(xmlbuf);
				}
				else
				{
					printf("\nError during convert message");
					exit(1);
				}
			}

		
		
		close(membersoc);
	
	
}
int main()
{

	read_xml("dump_msg.xml");
	return (0);
}
