/*
	Simple example to open a maximum of 4 devices - write some data then read it back.
	Shows one method of using list devices also.
	Assumes the devices have a loopback connector on them and they also have a serial number

	To build use the following gcc statement 
	(assuming you have the static d2xx library in the /usr/local/lib directory
	and you have created a symbolic link to it in the current dir).
	gcc -o static_link main.c -lpthread libftd2xx.a 
*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <unistd.h>
#include "ftd2xx.h"
#include <string.h>

#define BUF_SIZE 0x1

#define MAX_DEVICES		1

char initParameter(char **argv, int argc);
void printHelp();
void split(char* str, char* delimiter, char** retVars);
int contains(char* heyStack, char* needle);
int parseInt(char* str);

int main(int argc, char **argv)
{
	UCHAR 	cBufWrite[BUF_SIZE];
	char * 	pcBufLD[MAX_DEVICES + 1];
	char 	cBufLD[MAX_DEVICES][64];
	DWORD 	dwBytesWritten;
	FT_STATUS	ftStatus;
	FT_HANDLE	ftHandle[MAX_DEVICES];
	int	iNumDevs = 0;
	int	i;
	int	iDevicesOpen;	
	int number = 0;
	
	char inputParam = initParameter(argv, argc);
	if(inputParam == 0) {
		return 0;
	}
	printf("%d \n", inputParam);
	return 0;
	
	
	for(i = 0; i < MAX_DEVICES; i++) {
		pcBufLD[i] = cBufLD[i];
	}
	pcBufLD[MAX_DEVICES] = NULL;
	
	ftStatus = FT_ListDevices(pcBufLD, &iNumDevs, FT_LIST_ALL | FT_OPEN_BY_SERIAL_NUMBER);
	
	if(ftStatus != FT_OK) {
		printf("Error: FT_ListDevices returned %d\n", (int)ftStatus);
		return ftStatus;
	}
	
	if(iNumDevs <= 0) {
		printf("No devices found\n");
		return ftStatus;
	}		
	
	for(i = 0; ( (i <MAX_DEVICES) && (i < iNumDevs) ) ; i++) {
		printf("Device %d Serial Number - %s\n", i, cBufLD[i]);
		/* Setup */
		ftStatus = FT_OpenEx(cBufLD[i], FT_OPEN_BY_SERIAL_NUMBER, &ftHandle[i]);
		if(ftStatus != FT_OK){
			/* 
				This can fail if the ftdi_sio driver is loaded
		 		use lsmod to check this and rmmod ftdi_sio to remove
				also rmmod usbserial
		 	*/
			printf("Error: FT_OpenEx returned %d for device %d\n", (int)ftStatus, i);
			return ftStatus;
		}
	
		printf("Opened device %s\n", cBufLD[i]);
		
		ftStatus = FT_SetBitMode(ftHandle[i], 0xFF, FT_BITMODE_ASYNC_BITBANG);
		if (ftStatus != FT_OK) 
		{
			printf("FT_SetBitMode failed (error %d).\n", (int)ftStatus);
			return ftStatus;
		}
		
		/* Write */
		cBufWrite[0] = number;
		printf("Writing %d\n", cBufWrite[0] );
		ftStatus = FT_Write(ftHandle[i], cBufWrite, BUF_SIZE, &dwBytesWritten);
		if(ftStatus != FT_OK) {
			printf("Error: FT_Write returned %d\n", (int)ftStatus);
			break;
		}
		sleep(1);
	}

	iDevicesOpen = i;
	/* Cleanup */
	for(i = 0; i < iDevicesOpen; i++) {
		FT_Close(ftHandle[i]);
		printf("Closed device %s\n", cBufLD[i]);
	}

	return 0;
}

char initParameter(char **argv, int argc) {
	if(argc <= 1) {
		printHelp();
		return 0;
	}
	
	char retValue = 0;
	for(int i=1; i<argc;i++) {
		if(contains(argv[i], "--help")) {
			printHelp();
			return 0;
		}
		else if(contains(argv[i], "-s")) {
			if(argc > i + 1 && argc < 4) {
				int number = (int)parseInt(argv[i+1]);
				if(number == -1) {
					return 0;
				}
				retValue = number;
				
				return retValue;
			}
			else {
				printHelp();
				return 0;
			}
		}
		else if(argc > i + 1 && contains(argv[i], "-p") && !contains(argv[i+1], "-")) {
				char* value [5];
				split(argv[i+1], ":", value);
				int number  = 0;
				int iValue = 0, iIndex = 0;
				iValue = parseInt(value[1]);
				if(iValue == -1) {
					return 0;
				}
				iIndex = parseInt(value[0]);
				if(iIndex == -1) {
					return 0;
				}
				
				if(iValue < 0 || iValue > 1) {
					printHelp();
					return 0;
				}
				number = (iValue << (iIndex - 1));
				retValue |= number;
		}
		else {
			printHelp();
			return 0;
		}
	}

	return retValue;	
}

void printHelp() {
	printf("RelayControl       (Created by V-Modder)\n\n");
	printf("Usage:\trelaycontol OPTION...\n\n");
	printf("Options:\n");
	printf("\t-p [Pin]:[State]\tUsing pin mode, where each pin and state is set in the next argument\n");
	printf("\t-s 0xH\t\t\tUsing single mode, where all pins are set at once by the given hex-mask\n\n\n");
	printf("If -s parameter is set no other parameters are excepted\n");
	printf("In pin mode you can have n arguments.\n");
}

int contains(char* heyStack, char* needle) {
	if(heyStack == NULL || needle == NULL) {
		return 0;
	}
	if (strstr(heyStack, needle) != NULL) {
		return 1;
	}
	return 0;
}

void split(char* str, char* delimiter, char** retVars) {
	char* ptr;
	int i = 0;
	ptr = strtok(str, delimiter);
	
	while(ptr) {
		retVars[i++] = ptr;
		ptr = strtok(NULL, delimiter);
	}
}

int parseInt(char* str) {
	char *endptr;
	long number = strtol(str, &endptr, 10);
	
	if (str == endptr) {
        printf (" number : %lu  invalid  (no digits found, 0 returned)\n", number);
		number = -1;
	}
    else if (errno == ERANGE && number == LONG_MIN) {
        printf (" number : %lu  invalid  (underflow occurred)\n", number);
		number = -1;
	}
    else if (errno == ERANGE && number == LONG_MAX) {
        printf (" number : %lu  invalid  (overflow occurred)\n", number);
		number = -1;
	}
    else if (errno == EINVAL) { /* not in all c99 implementations - gcc OK */
        printf (" number : %lu  invalid  (base contains unsupported value)\n", number);
		number = -1;
	}
    else if (errno != 0 && number == 0) {
        printf (" number : %lu  invalid  (unspecified error occurred)\n", number);
		number = -1;
	}
    else if (errno == 0 && str && !*endptr) {
        printf (" number : %lu    valid  (and represents all characters read)\n", number);
		number = -1;
	}
    else if (errno == 0 && str && *endptr != 0) {
        printf (" number : %lu    valid  (but additional characters remain)\n", number);
		number = -1;
	}

	return number;
}
