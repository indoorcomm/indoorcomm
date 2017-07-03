/*
 * main.c
 *
 *  Created on: 30.06.2017
 *      Author: robert
 */


#include <stdio.h>
#include <stdlib.h>
//#include "wireless.h"
#include "iwlib.h"
#include "wlanscan.h"
#include "data_connection.h"
#include "bus_component/bus_handler.h"
#include "Odometrie.h"
#include "ScanHandler.h"
#include "FileHandler.h"
#include "data_connection.h"


int main(int argc, char*argv[]){
	int skfd =0;
	int timer;
	Position_t pos ={0,0};
	if((skfd = iw_sockets_open()) < 0)
		{
			perror("socket");
			return -1;
		}
	if (strcmp(argv[1], "Vermessung") == 0) {


		for(timer=0;timer<20;timer++){
			scanWlan(argc,argv,skfd,pos);
			usleep(1000000);
		}
	}else if(strcmp(argv[1], "Navigation") == 0){
		openFile("test.txt","r+");
		printf("Open File\n");
		PositionList_t positionList = readFromFile();
		printf("File read\n");
		closeFile();
		for(timer=0;timer<20;timer++){
			navigate(argc,argv,skfd,positionList);
			usleep(1000000);
		}
	}else{

	}
	iw_sockets_close(skfd);
	return 0;
}
