/*
 * gap_layer_app.h
 *
 *  Created on: 1 mar 2021
 *      Author: Jared
 */

#ifndef SF_APP_BLE_STACKS_CORES_GAP_LAYER_APP_H_
#define SF_APP_BLE_STACKS_CORES_GAP_LAYER_APP_H_

typedef struct
{
	uint8_t value_d;
	uint8_t value_u;
	uint8_t value_dt;
	uint8_t value_ut;
	uint8_t value_n;

}data_config_t;

data_config_t data_select[6];

#define BEACON_EDDYSTONE_TLM 4



/* User defined UUID for iBeacon */
#define UUID_IBEACON     0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f

char datafv_cfb[8]={0x4c,0x41,0x49,0x52,0x44,0x53,0x4c,0x50};
char datafv_cfbV[8]={0x4c,0x41,0x49,0x52,0x44,0x53,0x54,0x56};
char datac_cfb[6];
char dataa_cfb2[14];

extern unsigned char data_rssi_save1[10];

extern const wiced_bt_cfg_settings_t app_cfg_settings2;
extern const wiced_bt_cfg_buf_pool_t app_buf_pools2[];

void gap_stack_init(void);
void gap_transferA(void);
void gap_transferB(void);

static void                     beacon_init(void);
static wiced_result_t           beacon_management_callback(wiced_bt_management_evt_t event, wiced_bt_management_evt_data_t *p_event_data);
static void                     beacon_advertisement_stopped(void);
static void                     beacon_set_app_advertisement_data2();
void                            beacon_set_app_advertisement_data3(void);
void                            gap_transferER(void);
void                            set_data_base(void);

extern void	                    config_Transceiver(void);
extern void                     init_config_logs(void);
extern void                     init_mac_logs(void);
extern void                     start_observe(void);

extern void                     set_outPuts(void);
extern void                     set_intPuts(void);
extern void                     register_pin_interrupt(void);
extern void                     event_select_OTA(void);
extern void                     set_adc_p(void);
extern void                     event_select_SPP(void);
extern void                     set_rssi(void);

extern void                     config_clk_timers(void);
extern void 					init_Acarreo_parameters(void);
//extern void                     start_BTimers(void);

extern void process_SOM(uint8_t *data_S_OM);
extern void process_Write(uint8_t *data_Write);


#endif /* SF_APP_BLE_STACKS_CORES_GAP_LAYER_APP_H_ */
