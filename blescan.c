
#include <sys/time.h>
#include <stdlib.h>
#include <errno.h>
//#include <curses.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include "/usr/include/bluetooth/bluetooth.h" //aber wieso ? TODO
#include "/usr/include/bluetooth/hci.h"
#include "/usr/include/bluetooth/hci_lib.h"
//#include <bluetooth/bluetooth.h>
//#include <bluetooth/hci.h>
//#include <bluetooth/hci_lib.h>
#include <string.h>
#include "blescan.h"
#include <time.h>
static char BLEAddress[3][18];
static int BLEReadFlag=0;

struct hci_request ble_hci_request(uint16_t ocf, int clen, void * status, void * cparam)
{
	struct hci_request rq;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = OGF_LE_CTL;
	rq.ocf = ocf;
	rq.cparam = cparam;
	rq.clen = clen;
	rq.rparam = status;
	rq.rlen = 1;
	return rq;
}
 static int getBLEAddress(){
	FILE* fp= NULL;
	int i;
	int ret =0;
	bdaddr_t BLEAdresses[3];

	if ( (fp = fopen("bleaddresses.txt","r+")) == NULL){
		perror("Could not open bleaddresses");
		ret =0;
	}else{
		for(i=0;i<3;i++){
			printf("Read %d\n",i);
			if(fscanf(fp,"%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n",
					&BLEAdresses[i].b[0],&BLEAdresses[i].b[1],
			&BLEAdresses[i].b[2],&BLEAdresses[i].b[3],&BLEAdresses[i].b[4],&BLEAdresses[i].b[5]) == 6){
				printf("%d. Address succesfully read\n",i);
				if(i==2){ // All three addresses read
					ret =1;
					BLEReadFlag =1;
				}
			//ba2str(&BLEAdresses[i],BLEAddress[i]);
			sprintf(BLEAddress[i], "%2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X",BLEAdresses[i].b[0],BLEAdresses[i].b[1],
                        BLEAdresses[i].b[2],BLEAdresses[i].b[3],BLEAdresses[i].b[4],BLEAdresses[i].b[5]);
			printf("%s\n",BLEAddress[i]);
			}else{
				perror("Error reading BLE addresses from file");
				ret=0;
			}
		}
	}
	printf("close\n");
	fclose(fp);
	printf("FilePointer closed\n");
	return ret;
}


int bleScan(blescan_t* bleData)
{

	int ret, status,retV;
	struct timeval start,stop,diff;
	if(!BLEReadFlag){
		getBLEAddress();
	}
	// Get HCI device.
	printf("Opening HCI device\n");
	const int device = hci_open_dev(hci_get_route(NULL));
	if ( device < 0 ) { 
		perror("Failed to open HCI device.");
		return 0; 
	}

	// Set BLE scan parameters.
	printf("Setting Scan Parameters\n");
	le_set_scan_parameters_cp scan_params_cp;
	memset(&scan_params_cp, 0, sizeof(scan_params_cp));
	scan_params_cp.type 			= 0x00; 
	scan_params_cp.interval 		= htobs(0x0010);
	scan_params_cp.window 			= htobs(0x0010);
	scan_params_cp.own_bdaddr_type 	= 0x00; // Public Device Address (default).
	scan_params_cp.filter 			= 0x00; // Accept all.

	struct hci_request scan_params_rq = ble_hci_request(OCF_LE_SET_SCAN_PARAMETERS, LE_SET_SCAN_PARAMETERS_CP_SIZE, &status, &scan_params_cp);
	printf("Send Param request\n");
	ret = hci_send_req(device, &scan_params_rq, 1000);
	if ( ret < 0 ) {
		hci_close_dev(device);
		perror("Failed to set scan parameters data.");
		return 0;
	}

	// Set BLE events report mask.

	le_set_event_mask_cp event_mask_cp;
	memset(&event_mask_cp, 0, sizeof(le_set_event_mask_cp));
	int i = 0;
	for ( i = 0 ; i < 8 ; i++ ) event_mask_cp.mask[i] = 0xFF;

	struct hci_request set_mask_rq = ble_hci_request(OCF_LE_SET_EVENT_MASK, LE_SET_EVENT_MASK_CP_SIZE, &status, &event_mask_cp);
	ret = hci_send_req(device, &set_mask_rq, 1000);
	if ( ret < 0 ) {
		hci_close_dev(device);
		perror("Failed to set event mask.");
		return 0;
	}

	// Enable scanning.

	le_set_scan_enable_cp scan_cp;
	memset(&scan_cp, 0, sizeof(scan_cp));
	scan_cp.enable 		= 0x01;	// Enable flag.
	scan_cp.filter_dup 	= 0x00; // Filtering disabled.

	struct hci_request enable_adv_rq = ble_hci_request(OCF_LE_SET_SCAN_ENABLE, LE_SET_SCAN_ENABLE_CP_SIZE, &status, &scan_cp);

	ret = hci_send_req(device, &enable_adv_rq, 1000);
	if ( ret < 0 ) {
		hci_close_dev(device);
		perror("Failed to enable scan.");
		return 0;
	}

	// Get Results.

	struct hci_filter nf;
	hci_filter_clear(&nf);
	hci_filter_set_ptype(HCI_EVENT_PKT, &nf);
	hci_filter_set_event(EVT_LE_META_EVENT, &nf);
	if ( setsockopt(device, SOL_HCI, HCI_FILTER, &nf, sizeof(nf)) < 0 ) {
		hci_close_dev(device);
		perror("Could not set socket options\n");
		return 0;
	}

	printf("Scanning....\n");

	uint8_t buf[HCI_MAX_EVENT_SIZE];
	evt_le_meta_event * meta_event;
	le_advertising_info * info;
	int len;
	int scanCompleted =0;
	int addr1=0,addr2=0,addr3 =0;
	blescan_t blescan;
	gettimeofday(&start,NULL);
	while ( !scanCompleted ) {
		len = read(device, buf, sizeof(buf));
		if ( len >= HCI_EVENT_HDR_SIZE ) {
			meta_event = (evt_le_meta_event*)(buf+HCI_EVENT_HDR_SIZE+1);
			if ( meta_event->subevent == EVT_LE_ADVERTISING_REPORT ) {
				uint8_t reports_count = meta_event->data[0];
				void * offset = meta_event->data + 1;
				while ( reports_count-- ) {
					info = (le_advertising_info *)offset;
					char addr[18];
					ba2str(&(info->bdaddr), addr);

					printf("%s - RSSI %d\n", addr, (int8_t)info->data[info->length]);
					offset = info->data + info->length + 2;
						if(strcmp(addr,BLEAddress[0]) == 0){

								blescan.blescan[0].RSSI = (int8_t)info->data[info->length];
								addr1 =1;
						}else if(strcmp(addr,BLEAddress[1]) == 0){

								blescan.blescan[1].RSSI = (int8_t)info->data[info->length];
								addr2 =1;
						}else if(strcmp(addr,BLEAddress[2]) == 0){

								blescan.blescan[2].RSSI = (int8_t)info->data[info->length];
								addr3 =1;

						}
						if( addr1 && addr2 && addr3 ){
							scanCompleted = 1;
							retV =1;
						}
				gettimeofday(&stop,NULL);
				timersub(&stop,&start,&diff);
				if( (diff.tv_sec * 1000) + (diff.tv_usec / 1000) >= 10000){
					printf("Watchdog\n");
					 scanCompleted =1;
					retV = 0;
				}
				}
			}
		}
		gettimeofday(&stop,NULL);
		timersub(&stop,&start,&diff);
		if( (diff.tv_sec * 1000) + (diff.tv_usec / 1000) >= 10000){
			printf("Watchdog\n");
			 scanCompleted =1;
			retV = 0;
		}
	}
	printf("Scan completed\n");

	printf("%s - RSSI %d\n",BLEAddress[0],blescan.blescan[0].RSSI);

	printf("%s - RSSI %d\n", BLEAddress[1],blescan.blescan[1].RSSI);

	printf("%s - RSSI %d\n", BLEAddress[2],blescan.blescan[2].RSSI);

	bleData->blescan[0].RSSI = blescan.blescan[0].RSSI;
	strcpy(bleData->blescan[0].SSID,BLEAddress[0]);
	bleData->blescan[1].RSSI = blescan.blescan[1].RSSI;
	strcpy(bleData->blescan[1].SSID,BLEAddress[1]);
	bleData->blescan[2].RSSI = blescan.blescan[2].RSSI;
	strcpy(bleData->blescan[2].SSID,BLEAddress[2]);

	// Disable scanning.

	memset(&scan_cp, 0, sizeof(scan_cp));
	scan_cp.enable = 0x00;	// Disable flag.

	struct hci_request disable_adv_rq = ble_hci_request(OCF_LE_SET_SCAN_ENABLE, LE_SET_SCAN_ENABLE_CP_SIZE, &status, &scan_cp);
	ret = hci_send_req(device, &disable_adv_rq, 1000);
	if ( ret < 0 ) {
		hci_close_dev(device);
		perror("Failed to disable scan.");
		return 0;
	}

	hci_close_dev(device);
	
	return retV;
}

