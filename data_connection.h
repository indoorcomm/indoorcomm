/*
 * data_connection.h
 *
 *  Created on: 16.05.2017
 *      Author: Daniel
 */

#ifndef DATA_CONNECTION_H_
#define DATA_CONNECTION_H_

typedef struct AdressAndPower {
	int signalpower;
	char macadress[20];
} AdressAndPower_t;

typedef struct ListRow {
	double x;
	double y;
	AdressAndPower_t AdressAndPower[20];
} ListRow_t;

typedef struct PositionList {
	ListRow_t list[10];
} PositionList_t;

typedef struct Position {
	double x;
	double y;
} Position_t;

int minSignalPower(int array[], int signalpower);

int getAmountOfTwoSignals(int first, int second);

Position_t findPositionOfSignal(AdressAndPower_t searchedSignal[], PositionList_t positionList);



int getIndexByRegex(char row[], char regex, int beginningIndex);



#endif /* DATA_CONNECTION_H_ */
