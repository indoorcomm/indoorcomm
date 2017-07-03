/*
 * ScanHandler.c
 *
 *  Created on: 19.06.2017
 *      Author: Daniel
 */

#include "iwlib.h"
#include "data_connection.h"
#include "bus_component/bus_handler.h"
#include "wlanscan.h"
#include "FileHandler.h"
#include "blescan.h"

/**
 * Makes a scan and checks to the list of data. Prints the result.
 */
void navigate(int argc, char **argv, int skfd, PositionList_t positionList) {
	char *dev = "wlan0";
	char *cmd = "scan";
	char **args = argv +3 ;
	int count = argc -3;
	scanData_t retscanData[10];
	if (dev){


		(print_scanning_info)(skfd, dev, args, count,retscanData);

		int i;
		printf("Found Networks: \n");
		for(i =0; i<=10;i++){
			printf("ESSID: %s\n",retscanData[i].essid);
			printf("SCAN: %s,%d\n\n",retscanData[i].mac,retscanData[i].SSID);
		}
	}
	else{

		printf("ELSE!");
	}
	AdressAndPower_t adressPower[10];
	int j;
	for (j = 0; j < 10; j++) {
		//adressPower[j].macadress = retscanData[j].mac;
		strncpy(adressPower[j].macadress,retscanData[j].mac,sizeof(retscanData[j].mac));
		adressPower[j].signalpower = retscanData[j].SSID;
	}
	Position_t location;
	//run trough the whole list and check all incoming signals
	int posfound = 1;
	int i;
	for (i = 0; i < sizeof(positionList.list)/sizeof(positionList.list[0]); i++) {
		int deviceIndex;
		int counter = 0;
		//run through the devices at the position of the list
		for (deviceIndex = 0;
				deviceIndex < sizeof(positionList.list[i].AdressAndPower)/sizeof(positionList.list[i].AdressAndPower[0]);
				deviceIndex++) {
			int checkIndex;
			//run through the searched devices and compare to all. Searches with the one with the fitted macadress.
			if (counter == sizeof(adressPower)/sizeof(adressPower[0])) {
				location.x = positionList.list[i].x;
				location.y = positionList.list[i].y;
				posfound = 0;
				printf("\nPosition:%fl %fl\n", location.x, location.y);
				break;
			}
			int sSignal;
			int listSignal;
			for (checkIndex = 0; checkIndex < sizeof(adressPower)/sizeof(adressPower[0]); checkIndex++) {
				if (strcmp(positionList.list[i].AdressAndPower[deviceIndex].macadress, adressPower[1].macadress) == 0) {
					sSignal = adressPower[checkIndex].signalpower;
					listSignal = positionList.list[i].AdressAndPower[deviceIndex].signalpower;
					int amount = getAmountOfTwoSignals(listSignal, sSignal);
					if (listSignal == sSignal || (amount < 5 && amount >= 0)) {
						counter++;
						break;
					}
				}
			}
		}
		//if all signal are fitting save and exit.
		if(posfound == 0) {
			break;
		}
		counter = 0;
	}
}

/**
 * Scans the driver of the wlan and writes it with the position to the file.
 */
void scanWlan(int argc, char **argv, int skfd, Position_t position) {
	char *dev = "wlan0";
	char *cmd = "scan";
	char **args = argv +3 ;
	int count = argc -3;

	scanData_t retScanData[20];
	openFile("test.txt", "a+");
	int i;
	int validCount=0;
	/* do the actual work */
	if (dev){


		(print_scanning_info)(skfd, dev, args, count,retScanData);

		int i;
		printf("Found Networks: \n");
		for(i =0; i<=10;i++){
			printf("ESSID: %s\n",retScanData[i].essid);
			printf("SCAN: %s,%d\n\n",retScanData[i].mac,retScanData[i].SSID);
		}
	}
	else{

		printf("ELSE!");
	}
	ListRow_t listRow;
	blescan_t blescan;
	int j;
	validCount=0;
	for (j = 0; j < sizeof(retScanData)/sizeof(retScanData[0]); j++) {
		//listRow.AdressAndPower[j].macadress[0] = retScanData[j].mac;
		if (retScanData[j].validScan != 1){
			break;
		}
		validCount++;
		strncpy(listRow.AdressAndPower[j].macadress,retScanData[j].mac,sizeof(retScanData[j].mac));
		listRow.AdressAndPower[j].signalpower = retScanData[j].SSID;

	}
		if(bleScan(&blescan) > 0){
			for(j=0;j<3;j++){
			strcpy(listRow.AdressAndPower[validCount].macadress,blescan.blescan[0].SSID);
			listRow.AdressAndPower[validCount].signalpower = blescan.blescan[0].RSSI;
			validCount++;
			}
		}
	//Odomometrie
	listRow.x = position.x;
	listRow.y = position.y;

	for (i = 0; i < validCount/*(sizeof(listRow.AdressAndPower)/
			sizeof(listRow.AdressAndPower[0]))*/; i++) {
		writeMacadressAndSignalPower(listRow.AdressAndPower[i].macadress, listRow.AdressAndPower[i].signalpower);
	}
	writePosition(listRow.x, (float)validCount);//listRow.y);
	flushFile();
	//make here the calculation of the new position.
	usleep(1000000);
}
