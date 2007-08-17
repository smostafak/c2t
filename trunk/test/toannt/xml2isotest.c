#include <stdio.h>
#include <string.h>
#include "iso8583.h"
#include "iso8583_std.h"


int main(int argc, char *argv[]) {
	
	FILE *fp1, *fp2;
	int bufsize = 512;
	char buf[bufsize];
	int bmp_flag = 0;
	
	
	if (argc < 5) {
		printf("usage: xml2iso -option bmp_flag xmlfile isofile\n\t-option: -o xml to iso, -i iso to xml\n\tbmp_flag 0 or 1\n\ttxmlfile: the file stores xml data\n\tisofile: the file stored isomessage\n");
		return -1;
	}
	
	if (strcmp(argv[1],"-o") != 0 && strcmp(argv[1],"-i") != 0) {
		printf("invalid option: %s, must be -i or -o\n", argv[1]);		
		return -1;
	}
	
	bmp_flag = atoi(argv[2]);
	
	if (!strcmp(argv[1],"-o")) {
		fp1 = fopen(argv[3], "r");
		if (fp1 == NULL) {
			printf("file: %s not found!", argv[2]);
			return -1;
		}
		fp2 = fopen(argv[4], "a+");
		
		char xml[4000];
		int len = 0;
		
		while (!feof(fp1)) {
			len = fread(xml + len, sizeof(char), bufsize, fp1);
		}
		
		printf("input xml: %s\n", xml);
		
		char *iso = xml_to_iso(xml, iso87, bmp_flag);
		
		if (iso == NULL) {
			printf("Convert error!");
		}
		else {
			printf("output iso: %s\n", iso);
			isomsg m;
			init_message(&m, bmp_flag);
			if (!unpack_message(&m, iso87, iso)) {
				int i = 0;
				for (i = 0; i <= 128;i++) {
					if (m.fld[i] != NULL) {
						printf("field #%d = %s\n", i, m.fld[i]);
					}
				}
			} 
			else {
				printf("unpack message not succeedful!");
			}
			free_message(&m);
			fwrite(iso, sizeof(char), strlen(iso), fp2);
			free(iso);
		}
	}
	else {
		fp1 = fopen(argv[4], "r");
		if (fp1 == NULL) {
			printf("file: %s not found!", argv[4]);
			return -1;
		}
		fp2 = fopen(argv[3], "a+");
		
		char iso[4000];
		int len = 0;
		
		while (!feof(fp1)) {
			len = fread(iso + len, sizeof(char), bufsize, fp1);
		}
		
		printf("input iso: %s\n", iso);
		
		char *xml = iso_to_xml (iso, iso87, bmp_flag);
		
		if (xml == NULL) {
			printf("Convert error!");
		}
		else {
			printf("output xml: %s\n", xml);
			fwrite(xml, sizeof(char), strlen(iso), fp2);
			free(xml);
		}
	}
	
	fclose(fp1);
	fclose(fp2);
	
	return 0;
}
