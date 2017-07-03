/*
 * ScanHandler.h
 *
 *  Created on: 19.06.2017
 *      Author: Daniel
 */

#ifndef SCANHANDLER_H_
#define SCANHANDLER_H_

void navigate(int argc, char **argv, int skfd,PositionList_t positionList);

void scanWlan(int argc, char **argv, int skfd, Position_t position);

#endif /* SCANHANDLER_H_ */
