/*
 * Odometrie.h
 *
 *  Created on: 16.05.2017
 *      Author: Daniel
 */

#ifndef ODOMETRIE_H_
#define ODOMETRIE_H_

typedef struct PositionNav{
	double x;
	double y;
}PositionNav_t;

typedef struct KalmanParameters {
	Position_t position;
	Position_t errorPosition;
	double angle;
	double errorAngle;
	double Pk[3][3];
}KalmanParameter_t;

Position_t KoppelNavigation(Position_t oldPosition, double distance, double angle);

KalmanParameter_t KalmanFilter(KalmanParameter_t kalmanParameters, double *F[], double *H[]);



#endif /* ODOMETRIE_H_ */
