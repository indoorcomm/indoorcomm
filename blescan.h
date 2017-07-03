/*
 * blescan.h
 *
 *  Created on: 29.06.2017
 *      Author: robert
 */

#ifndef BLESCAN_H_
#define BLESCAN_H_

 struct blescan{
	char SSID[18];
	int RSSI;
};
 typedef struct{
	 struct blescan blescan[3];
 }blescan_t;

 int bleScan(blescan_t* bleData);
#endif /* BLESCAN_H_ */
