/*
 * wlanscan.h
 *
 *  Created on: 01.05.2017
 *      Author: robert
 */

#ifndef WLANSCAN_H_
#define WLANSCAN_H_

typedef struct iwscan_state
{
  /* State */
  int			ap_num;		/* Access Point number 1->N */
  int			val_index;	/* Value in table 0->(N-1) */
} iwscan_state;

typedef struct scanData
{
	uint8_t validScan;
	char mac[20];
	int SSID;
	char essid[33];

}scanData_t;

int print_scanning_info(int		skfd,
		    char *	ifname,
		    char *	args[],		/* Command line args */
		    int		count,
			scanData_t* ret);		/* Args count */


#endif /* WLANSCAN_H_ */
