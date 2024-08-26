/*
 * localization.h
 *
 *  Created on: 21 ago 2024
 *      Author: Usuario1
 */

#ifndef SF_APP_BLE_PROCESSES_BLE_LOCALIZATION_H_
#define SF_APP_BLE_PROCESSES_BLE_LOCALIZATION_H_

#include "wiced_bt_ble.h"
#include "wiced_timer.h"
#include "wiced_bt_trace.h"
#include <string.h>
#include "wiced_hal_gpio.h"
#include "config_ports.h"


/** Filters Beacons for UID */
uint8_t Filt_UUID1[3] = { 0x42, 0x53, 0x31 };		// Filter ( BEACON1 - BS1 )
uint8_t Filt_UUID2[3] = { 0x42, 0x53, 0x32 };		// Filter ( BEACON2 - BS2 )
uint8_t Filt_UUID3[3] = { 0x42, 0x53, 0x33 };		// Filter ( BEACON3 - BS3 )

uint8_t	Filt_ID[2] = {0x42, 0x4e};					// Filter ( BN )
char scanner_url[8];
extern void Put_in_beacon(uint8_t *P_punteroURL,wiced_bt_ble_scan_results_t *p_scan_result);
extern void restart_timer(void);
B_localizacion Data_BecLoc;

#endif /* SF_APP_BLE_PROCESSES_BLE_LOCALIZATION_H_ */
