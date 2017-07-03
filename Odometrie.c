/*
 * Odometrie.c
 *
 *  Created on: 06.05.2017
 *      Author: Daniel
 */

#include<stdio.h>
#include<stdlib.h>
#include<math.h>

typedef struct Position{
	double x;
	double y;
}Position_t;

typedef struct KalmanParameters {
	Position_t position;
	Position_t errorPosition;
	double angle;
	double errorAngle;
	double Pk[3][3];
}KalmanParameter_t;

/**
 * Koppelnavigation
 */
Position_t KoppelNavigation(Position_t oldPosition, double distance, double angle) {
	Position_t newPosition;
	newPosition.x = oldPosition.x + distance * cos(angle);
	newPosition.y = oldPosition.y + distance * sin(angle);
	return newPosition;
}

KalmanParameter_t KalmanFilter(KalmanParameter_t kalmanParameters, double *F[], double *H[]) {
	double unityMatrix[3][3] = {{1, 0, 0}, {1, 0, 0}, {1, 0, 0}};
	double rk[3][3] = {{0.3, 0, 0}, {0, 0.3, 0}, {0, 0, 0.3}};
	double Pk[3][3];
	Pk[3][3] = kalmanParameters.Pk[3][3];
	double kk[3][3];
	double rg[3][3];
	double ig[3][3];
	//change zk to three on
	double zk[3][1] = {{kalmanParameters.errorPosition.x}, {kalmanParameters.errorPosition.y}, {kalmanParameters.errorAngle}};
	double xk[3][1] = {kalmanParameters.position.x, kalmanParameters.position.y, kalmanParameters.angle};
	xk[3][1] = multiplicate3x3(xk, F);
	Pk[3][3] = multiplicate3x3(multiplicate3x3(F, Pk), transpose(F));
	rg[3][3] = add3x3(invert(multiplicate3x3(multiplicate3x3(H, Pk), transpose(H))), rk);
	kk[3][3] = multiplicate3x3(multiplicate3x3(Pk, transpose(H)), rg);
	ig[3][3] = subtract3x3(zk, multiplicate3x3(H, xk));
	xk[3][1] = add3x3(xk, multiplicate3x3(kk, ig));
	Pk[3][3] = multiplicate3x3(subtract3x3(unityMatrix, multiplicate3x3(kk, H)), Pk);
	//intitialize new parameters
	kalmanParameters.Pk[3][3] = Pk[3][3];
	kalmanParameters.position.x = xk[1][1];
	kalmanParameters.position.y = xk[2][1];
	kalmanParameters.angle = xk[3][1];
	return kalmanParameters;
}

int transpose(int *matrix[]) {
	int transposedMatrix[sizeof(matrix)][sizeof(sizeof(matrix))];
	int i;
	for(i = 0; i < 3; i++) {
		int j;
		for (j = 0; j < 3; j++) {
			transposedMatrix[j][i] = matrix[i][j];
		}
	}
	return transposedMatrix;
}

multiplicate3x3(double *firstMatrix[], double *secondMatrix[]) {
	double multiplicatedMatrix[sizeof(firstMatrix)][sizeof(sizeof(firstMatrix))];
	int i;
	for (i = 0; i < sizeof(firstMatrix); i++) {
		double multiLine;
		int j;
		for (j = 0; j < sizeof(sizeof(firstMatrix)); j++) {
			multiLine += firstMatrix[i][j] * secondMatrix[j][i];
		}
		multiplicatedMatrix[i][0] = multiLine;
	}
	return multiplicatedMatrix;//multiplicatedMatrix;
}

add3x3(int *firstMatrix[], int *secondMatrix[]) {
	int addMatrix[sizeof(firstMatrix)][sizeof(sizeof(firstMatrix))];
	int i;
	for (i = 0; i < sizeof(firstMatrix); i++) {
		int j;
		for (j = 0; j < sizeof(sizeof(firstMatrix)); j++) {
			addMatrix[i][j] = firstMatrix[i][j] + secondMatrix[i][j];
		}
	}
	return addMatrix;
}

subtract3x3(int *firstMatrix[], int *secondMatrix[]) {
	int subtractMatrix[sizeof(firstMatrix)][sizeof(sizeof(firstMatrix))];
	int i;
	for (i = 0; i < sizeof(firstMatrix); i++) {
		int j;
		for (j = 0; j < sizeof(sizeof(firstMatrix)); j++) {
			subtractMatrix[i][j] = firstMatrix[i][j] - secondMatrix[i][j];
		}
	}
	return subtractMatrix;
}
invert(double *matrix[]) {

}
