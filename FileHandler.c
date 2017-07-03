/*
 * FileHandler.c
 *
 *  Created on: 19.06.2017
 *      Author: Daniel
 */
#include <string.h>
#include "data_connection.h"

#include <stdio.h>
#include <stdlib.h>

static FILE *fp;

void openFile(char fileName[100],char mode[2]) {
	fp = fopen(fileName, mode);
}

void writeMacadressAndSignalPower(char macAdress[20], int signalPower) {
	fprintf(fp, "%s %d,", macAdress, signalPower);
}

void writeWordWrap() {
	fprintf(fp, "\n");
}

void writePosition(double xPos, double yPos) {
	fprintf(fp, "%d,%d\n", (int)xPos, (int)yPos);
}

void flushFile() {
	fflush(fp);
}

void closeFile() {
	fclose(fp);
}

PositionList_t readFromFile() {
	PositionList_t positionList;
	char macAdress[20];
	char signal[5];
	char position[5];
	int signalPower;
	double x;
	double y;
	int index = 0;
	int deviceIndex = 0;
	int charIndex = 0;
	int c;
	int macOrPower = 0;
	int counter = 0;
	char *pt;
	//int posCounter = 0;
	//read the file
	//printf("Vor while\n");
	while(!feof(fp)) {
		//printf("in while\n");
		c = fgetc(fp);
		//printf("fgetc\n");
		if (feof(fp)) {
			break;
		}
		if (c == ' ') {
			strncpy(positionList.list[index].AdressAndPower[deviceIndex].macadress, macAdress, sizeof(macAdress));
			charIndex = 0;
			macOrPower = 1;
			printf("Mac:%s \n", positionList.list[index].AdressAndPower[deviceIndex].macadress);
			continue;
		}
		if (c == ',') {
			if (counter >= 12) {
				signalPower = atoi(signal);
				positionList.list[index].AdressAndPower[deviceIndex].signalpower = signalPower;
				deviceIndex++;
				counter = 0;
				macOrPower = 0;
				printf("Signalpower:%d\n,", positionList.list[index].AdressAndPower[deviceIndex].signalpower);
			} else {
				x = strtod(position, &pt);
				positionList.list[index].x = x;
				counter = 0;
				printf("%lf,\n", positionList.list[index].x);
			}
			charIndex = 0;
			continue;
		}
		if (c == '\n') {
			y = strtod(position, &pt);
			positionList.list[index].y = y;
			counter = 0;
			macOrPower = 0;
			printf("%lf\n", positionList.list[index].y);
			index++;
			deviceIndex = 0;
			charIndex = 0;
			continue;
		}
		counter++;
		if (macOrPower == 0) {
			macAdress[charIndex] = c;
		} else if(macOrPower == 1) {
			signal[charIndex] = c;
			position[charIndex] = c;
		}
		charIndex++;
	}
	return positionList;
}
