/*
 * data_connection.c
 *
 *  Created on: 03.05.2017
 *      Author: Daniel
 */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#define ROWSIZE 50

typedef struct AdressAndPower {
	int signalpower;
	char macadress[20];
} AdressAndPower_t;

/**
 * Struct that contains the data to be saved.
 */
typedef struct ListRow {
	double x;
	double y;
	AdressAndPower_t AdressAndPower[13];
} ListRow_t;

/**
 * This is the list containing the saved data.
 */
typedef struct PositionList {
	ListRow_t list[20];
} PositionList_t;

/**
 * This struct contins the data to determine a position.
 */
typedef struct Position {
	double x;
	double y;
} Position_t;

/**
 * Searches the signal in the array next to the searched signal.
 */
int minSignalPower(int array[], int signalpower) {
	int min = array[0] + signalpower;
	int i;
	for (i = 1; i < sizeof(array); i++) {
		if ((array[i] + signalpower) < min) {
			min = array[i];
		}
	}
	return min;
}

/**
 * Gets the amount of the two signals. Checks if they are near together.
 */
int getAmountOfTwoSignals(int first, int second) {
	int amount;
	if (first < 0 && second < 0) {
		if (first < second) {
			amount = (first + second)*(-1);
		} else {
			amount = (second + first)*(-1);
		}
	} else if (first > 0 && second > 0) {
		if (first < second) {
			amount = second - first;
		} else {
			amount = first - second;
		}
	} else {
		amount = 100;
	}
	return amount;
}

/**
 * Finds the position with the belonging signalpower.
 */
Position_t findPositionOfSignal(AdressAndPower_t searchedSignal[], PositionList_t positionList) {
	Position_t position;
	//run trough the whole list and check all incoming signals
	int i;
	for (i = 0; i < sizeof(positionList.list); i++) {
		int deviceIndex;
		int counter = 0;
		//run through the devices at the position of the list
		for (deviceIndex = 0;
				deviceIndex < sizeof(positionList.list[i].AdressAndPower);
				deviceIndex++) {
			int checkIndex;
			//run through the searched devices and compare to all. Searches with the one with the fitted macadress.
			for (checkIndex = 0; checkIndex < sizeof(searchedSignal); checkIndex++) {
				if (positionList.list[i].AdressAndPower[deviceIndex].macadress == searchedSignal[checkIndex].macadress) {
					int sSignal = searchedSignal[checkIndex].signalpower;
					int listSignal = positionList.list[i].AdressAndPower[deviceIndex].signalpower;
					int amount = getAmountOfTwoSignals(listSignal, sSignal);
					if (listSignal == sSignal || amount < 5) {
						counter++;
						break;
					}
				}
			}
		}
		//if all signal are fitting save and exit.
		if (counter == sizeof(searchedSignal)) {
			position.x = positionList.list[i].x;
			position.y = positionList.list[i].y;
			break;
		}
	}
	return position;
}

/**
 * Creates the row which will be written into the file. For structure look at docs of write. "el" = end of line.
 */
void createRow(ListRow_t listRow,FILE* fp) {
	//char row[300];
	int i;

	printf("createRow %p\n", fp);
	for (i = 0; i < (sizeof(listRow.AdressAndPower)/
			sizeof(listRow.AdressAndPower[0])); i++) {
		printf("In for\n");
		/*char macAdress[22];
		char signalPower[5];
		char important[27];
		macAdress = strcat(listRow.AdressAndPower[i].macadress, " ");
		signalPower = strcat(listRow.AdressAndPower[i].signalpower, ",");
		important = strcat(macAdress, signalPower);
		row = strcat(row, important);*/
		printf("%d",listRow.AdressAndPower[i].signalpower);
		printf("%s",listRow.AdressAndPower[i].macadress);



		printf("nach fprintf\n");
	}
	/*char position[10];
	char positionX[5];
	char positionY[5];
	char xPos[sizeof(listRow.x)];
	double yPos[sizeof(listRow.y)];
	positionX = strcat(xPos, ",");
	positionY = strcat(yPos, "\n");
	position = strcat(positionX, positionY);*/
	printf("cor pos\n");
	fprintf(fp,"%d,%d\n",1,2);
	fflush(fp);
	//return row;
}

/**
 * Gets the index of a regex within a char*. You can begin at an index you want.
 */
int getIndexByRegex(char row[], char *regex, int beginningIndex) {
	int index = 0;
	int i;
	for (i = beginningIndex; i < sizeof(row); i++) {
		if (row[i] == regex[0]) {
			index = i;
			break;
		}
	}
	return index;
}

/**
 * Reads the whole data out of the file.
 */
PositionList_t readALL(FILE *fp) {
	PositionList_t positionList;

	char *buffer;
	fread(buffer, 10, 1, fp);
	int begin = 0;
	//run through the whole file an check every char
	int line = 0;
	int i;
	for (i = 0; i < sizeof(buffer); i++) {
		//check for end of the file.
		if (i + 1 < sizeof(buffer)) {
			//search for the end of the row
			char * endOfLine = buffer[i] && buffer[i+1];
			//get the whole row.
			char *row;
			int lineIndex;
			for (lineIndex = begin; lineIndex < i; lineIndex++) {
				row[lineIndex] = buffer[i];
			}
			i = sizeof(row);
			int deviceCount = 0;
			//get data out of line
			int rowCharIndex;
			for (rowCharIndex = 0; rowCharIndex < sizeof(row);
					rowCharIndex++) {
				//write macadress and signal and also postion in positionList.
				//macAdress
				int macAdressIndex = getIndexByRegex(row, " ",
						rowCharIndex);
				// no new macadress found. so its time for position
				if (macAdressIndex == 0) {
					int xPositonIndex = getIndexByRegex(row, ",",
							rowCharIndex);
					char *xposChar;
					int xpos;
					for (xpos = rowCharIndex; xpos < xPositonIndex;
							xpos++) {
						xposChar[xpos] = row[xpos];
					}
					positionList.list[line].x = atof(xposChar);
					rowCharIndex = xPositonIndex;
					int yPosition = getIndexByRegex(row, "n", rowCharIndex);
					char *yposChar;
					int ypos;
					for (ypos = rowCharIndex; ypos < yPosition-1;
							ypos++) {
						yposChar[ypos] = row[ypos];
					}
					positionList.list[line].y = atof(yposChar);
				} else {
					int adressIndex;
					for (adressIndex = rowCharIndex;
							adressIndex < macAdressIndex; adressIndex++) {
						positionList.list[line].AdressAndPower[deviceCount].macadress[adressIndex] =
								row[macAdressIndex];
					}
					rowCharIndex = macAdressIndex;
					deviceCount++;
					//signalPower
					int signalPowerIndex = getIndexByRegex(row, ",",
							rowCharIndex);
					int powerIndex;
					for (powerIndex = rowCharIndex;
							powerIndex < signalPowerIndex; powerIndex++) {
						positionList.list[line].AdressAndPower[deviceCount].macadress[powerIndex] =
								row[powerIndex];
					}
				}
				begin = i + 2;
				line++;
				continue;
			}
		} else {
			break;
		}
	}
	return positionList;
}
