/*
 * gap_layer_app.c
 *
 *  Created on: 1 mar 2021
 *      Author: Jared
 */



#include "wiced_hal_wdog.h"
#include "wiced_hal_rand.h"
#include "wiced_bt_stack.h"
#include "wiced_timer.h"
#include "wiced_bt_beacon.h"
#include "string.h"
#include "wiced_hal_puart.h"
#include "wiced_platform.h"
#include "wiced_bt_mesh_models.h"
#include "wiced_bt_mesh_app.h"
#include "wiced_bt_mesh_core.h"
#include "wiced_hal_gpio.h"
#include "wiced_hal_mia.h"
#include "wiced_gki.h"
#include "wiced_bt_stack.h"
#include "gap_layer_app.h"
#include "wiced_hal_nvram.h"
#include "wiced_bt_trace.h"
#include "config_timers.h"
#include "wiced_rtos.h"
#include "cycfg.h"
#include "cycfg_gatt_db.h"
#include "app_bt_utils.h"
#include "wiced_transport.h"
#include "sparcommon.h"




static wiced_bt_gatt_status_t	app_gatt_callback( wiced_bt_gatt_evt_t event, wiced_bt_gatt_event_data_t *p_data );

static wiced_bt_gatt_status_t	app_gatt_get_value( wiced_bt_gatt_read_t *p_data );
static wiced_bt_gatt_status_t	app_gatt_set_value( wiced_bt_gatt_write_t *p_data );
/*#if defined(CYW20735B1) || defined(CYW20819A1) || defined(CYW20719B2) || defined(CYW20721B2) || defined (WICEDX)

wiced_bt_ble_multi_adv_params_t adv_param =
#else
wiced_bt_beacon_multi_advert_data_t adv_param =
#endif
{
    .adv_int_min = BTM_BLE_ADVERT_INTERVAL_MIN,
    .adv_int_max = BTM_BLE_ADVERT_INTERVAL_MAX,
    .adv_type = MULTI_ADVERT_NONCONNECTABLE_EVENT,
    .channel_map = BTM_BLE_ADVERT_CHNL_37 | BTM_BLE_ADVERT_CHNL_38 | BTM_BLE_ADVERT_CHNL_39,
    .adv_filter_policy = BTM_BLE_ADVERT_FILTER_WHITELIST_CONNECTION_REQ_WHITELIST_SCAN_REQ,
    .adv_tx_power = MULTI_ADV_TX_POWER_MAX,
    .peer_bd_addr = {0},
    .peer_addr_type = BLE_ADDR_PUBLIC,
    .own_bd_addr = {0},
    .own_addr_type = BLE_ADDR_PUBLIC
};*/
/* Beacon timer */
static wiced_timer_t beacon_timer;
uint16_t      beacon_conn_id = 0;

/*************************************************************************************
 * Function name: gap_stack_init
 * Description: initialize the application stack for vehicle tag
 **************************************************************************************/
void gap_stack_init(void)
{
  init_config_logs();
  wiced_bt_stack_init(beacon_management_callback, &app_cfg_settings2, app_buf_pools2);
}

/*************************************************************************************
 * Function name:beacon_init
 * Description: This function is executed in the BTM_ENABLED_EVT management callback.
 * configures connectivity parameters, gap transmission layer data and
 * registers to configure the transmission name and its mac address
 **************************************************************************************/
void beacon_init(void)
{
    wiced_bt_gatt_status_t gatt_status;
    wiced_result_t         result;

#ifdef CYW20706A2
#if defined(USE_256K_SECTOR_SIZE)
    wiced_hal_sflash_use_erase_sector_size_256K(1);
    wiced_hal_sflash_use_4_byte_address(1);
#endif
#endif


    //-----------------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------------

    wiced_bt_dev_register_hci_trace(NULL);
    /* Allow peer to pair */
    wiced_bt_set_pairable_mode(WICED_TRUE, 0);
    /*
    WICED_BT_TRACE( "DATA 2 \n");
     numbytes = wiced_hal_read_nvram( WICED_NVRAM_VSID_START, sizeof(data_save_flash), &data_save_flash, &status );
     WICED_BT_TRACE_ARRAY( data_save_flash, 8 ,"DATA SAVED HEX: ");
       WICED_BT_TRACE( "\n");
       WICED_BT_TRACE( "DATA SAVED STRING: ");
       for(int i=0;i<8; i++){wiced_hal_puart_write(data_save_flash[i]);}
       WICED_BT_TRACE( "\n");
     */

//#if OTA_FW_UPGRADE
    /* OTA Firmware upgrade Initialization */
/*#ifdef OTA_SECURE_FIRMWARE_UPGRADE
    if (!wiced_ota_fw_upgrade_init(&ecdsa256_public_key, NULL, NULL))
#else
    if (!wiced_ota_fw_upgrade_init(NULL, NULL, NULL))
#endif
    {
          WICED_BT_TRACE("OTA upgrade Init failure !!! \n");
    }
#endif*/
    /* Set the advertising params and make the device discoverable */
    beacon_set_app_advertisement_data2();

    //-----------------------------------------------------------------------------------------------
     //set_adc_p();
     init_mac_logs();
    //-----------------------------------------------------------------------------------------------

    result =  wiced_bt_start_advertisements(BTM_BLE_ADVERT_UNDIRECTED_HIGH, 0, NULL);
    WICED_BT_TRACE("wiced_bt_start_advertisements %d\n", result);
}

/****************************************************************************************************************
 * Function name: beacon_management_callback
 * Description:   controls all general events, and allows changing to serial port configuration mode
 * @parameter     event: event number
 * @parameter     p_event_data: Structure definitions for Bluetooth Management
 ****************************************************************************************************************/
wiced_result_t beacon_management_callback(wiced_bt_management_evt_t event, wiced_bt_management_evt_data_t *p_event_data)
{
    wiced_result_t                    result = WICED_BT_SUCCESS;
    uint8_t                          *p_keys;
    wiced_bt_ble_advert_mode_t       *p_mode;
    uint8_t close_interval= 31;
    WICED_BT_TRACE("beacon_management_callback: %x\n", event);
    if(close_interval== event)
    {
    	WICED_BT_TRACE("beacon_management_callback2: %x\n", event);
    	//event_select_OTA();
    	//event_select_SPP();
    }
    switch(event)
    {
    /* Bluetooth  stack enabled */
    case BTM_ENABLED_EVT:
        beacon_init();
        config_Transceiver();
        start_observe();
        set_outPuts();
        //set_intPuts();
        //register_pin_interrupt();
        config_clk_timers();
        start_BTimers();
        set_rssi();
        init_Acarreo_parameters();

        WICED_BT_TRACE("FLAG_RSSI: %B\n", data_rssi_save1);
        //--------------------------------------------

		wiced_bt_gatt_register( app_gatt_callback );
		wiced_bt_gatt_db_init( gatt_database, gatt_database_len );

		/* Enable/disable pairing */
		wiced_bt_set_pairable_mode( WICED_FALSE, WICED_FALSE );
        set_data_base();
        //-----------------------------------------
        /* Configure LED PIN as input and initial outvalue as high */
        //wiced_hal_gpio_configure_pin( LED_GPIO_1, GPIO_OUTPUT_ENABLE, GPIO_PIN_OUTPUT_LOW );
        //wiced_hal_gpio_set_pin_output( LED_GPIO_1, GPIO_PIN_OUTPUT_LOW);
        break;

    case BTM_DISABLED_EVT:
        break;

    case BTM_PAIRING_IO_CAPABILITIES_BLE_REQUEST_EVT:
        p_event_data->pairing_io_capabilities_ble_request.local_io_cap  = BTM_IO_CAPABILITIES_NONE;
        p_event_data->pairing_io_capabilities_ble_request.oob_data      = BTM_OOB_NONE;
        p_event_data->pairing_io_capabilities_ble_request.auth_req      = BTM_LE_AUTH_REQ_BOND | BTM_LE_AUTH_REQ_MITM;
        p_event_data->pairing_io_capabilities_ble_request.max_key_size  = 0x10;
        p_event_data->pairing_io_capabilities_ble_request.init_keys     = BTM_LE_KEY_PENC | BTM_LE_KEY_PID;
        p_event_data->pairing_io_capabilities_ble_request.resp_keys     = BTM_LE_KEY_PENC | BTM_LE_KEY_PID;
        break;

    case BTM_SECURITY_REQUEST_EVT:
        wiced_bt_ble_security_grant( p_event_data->security_request.bd_addr, WICED_BT_SUCCESS );
        break;

    case BTM_PAIRED_DEVICE_LINK_KEYS_UPDATE_EVT:
        break;

     case BTM_LOCAL_IDENTITY_KEYS_UPDATE_EVT:
         break;


     case BTM_LOCAL_IDENTITY_KEYS_REQUEST_EVT:
        break;

    case BTM_BLE_ADVERT_STATE_CHANGED_EVT:
        p_mode = &p_event_data->ble_advert_state_changed;
        WICED_BT_TRACE("Advertisement State Change: %d\n", *p_mode);
        if (*p_mode == BTM_BLE_ADVERT_OFF)
        {
            beacon_advertisement_stopped();
        }
        break;

    default:
        break;
    }

    return result;
}

//---------------------------------------------------------------------


/*******************************************************************************
* Function Name: wiced_bt_gatt_status_t app_gatt_callback(
* 					wiced_bt_gatt_evt_t event,
* 					wiced_bt_gatt_event_data_t *p_data )
********************************************************************************/
wiced_bt_gatt_status_t app_gatt_callback( wiced_bt_gatt_evt_t event, wiced_bt_gatt_event_data_t *p_data )
{
    wiced_bt_gatt_status_t result = WICED_SUCCESS;

    wiced_bt_gatt_connection_status_t *p_conn = &p_data->connection_status;
    wiced_bt_gatt_attribute_request_t *p_attr = &p_data->attribute_request;

    switch( event )
    {
        case GATT_CONNECTION_STATUS_EVT:					// Remote device initiates connect/disconnect
            if( p_conn->connected )
			{
				WICED_BT_TRACE( "GATT connect to: BDA %B, Connection ID %d\r\n",p_conn->bd_addr, p_conn->conn_id );

				/* TODO Ex 02: Handle the connection */
			}
			else
			{
				// Device has disconnected
				WICED_BT_TRACE("GATT disconnect from: BDA %B, Connection ID '%d', Reason '%s'\r\n", p_conn->bd_addr, p_conn->conn_id, get_bt_gatt_disconn_reason_name(p_conn->reason) );

				/* TODO Ex 02: Handle the disconnection */

				/* Restart the advertisements */
				wiced_bt_start_advertisements( BTM_BLE_ADVERT_UNDIRECTED_HIGH, 0, NULL );
			}
            break;

        case GATT_ATTRIBUTE_REQUEST_EVT:					// Remote device initiates a GATT read/write
			switch( p_attr->request_type )
			{
				case GATTS_REQ_TYPE_READ:
					result = app_gatt_get_value( &(p_attr->data.read_req) );
					break;

				case GATTS_REQ_TYPE_WRITE:
					result = app_gatt_set_value( &(p_attr->data.write_req) );
					break;
            }
            break;

        default:
            break;
    }

    return result;
}


/*******************************************************************************
* Function Name: app_gatt_get_value(
* 					wiced_bt_gatt_read_t *p_data )
********************************************************************************/
static wiced_bt_gatt_status_t	app_gatt_get_value( wiced_bt_gatt_read_t *p_data )
{
	uint16_t attr_handle = 	p_data->handle;
	uint8_t  *p_val = 		p_data->p_val;
	uint16_t *p_len = 		p_data->p_val_len;
	uint16_t  offset =		p_data->offset;

	int i = 0;
	int len_to_copy;

    wiced_bt_gatt_status_t res = WICED_BT_GATT_INVALID_HANDLE;

    // Check for a matching handle entry
    for (i = 0; i < app_gatt_db_ext_attr_tbl_size; i++)
    {
    	// Search for a matching handle in the external lookup table
    	if (app_gatt_db_ext_attr_tbl[i].handle == attr_handle)
        {
            /* Start by assuming we will copy entire value */
    		len_to_copy = app_gatt_db_ext_attr_tbl[i].cur_len;

    		/* Offset is beyond the end of the actual data length, nothing to do*/
    		if ( offset >= len_to_copy)
    		{
    			return WICED_BT_GATT_INVALID_OFFSET;
    		}

    		/* Only need to copy from offset to the end */
    		len_to_copy = len_to_copy - offset;

    		/* Determine if there is enough space to copy the entire value.
    		 * If not, only copy as much as will fit. */
            if (len_to_copy > *p_len)
            {
            	len_to_copy = *p_len;
            }

			/* Tell the stack how much will be copied to the buffer and then do the copy */
			*p_len = len_to_copy;
			memcpy(p_val, app_gatt_db_ext_attr_tbl[i].p_data + offset, len_to_copy);
			res = WICED_BT_GATT_SUCCESS;

            // Add code for any action required when this attribute is read
            switch ( attr_handle )
            {
				case HDLC_STV_CONFIG_VALUE:
					WICED_BT_TRACE( "LED is %s\r\n", app_stv_config[0] ? "ON" : "OFF" );
					break;
            }
			break; /* break out of for loop once matching handle is found */
       }
    }
    return res;
}


/*******************************************************************************
* Function Name: app_gatt_set_value(
*					wiced_bt_gatt_write_t *p_data )
* @parameter: p_data: structure of the database,
*             in terms of data, length, handlers, etc.
* Description: access the database to display the saved values
********************************************************************************/
static wiced_bt_gatt_status_t	app_gatt_set_value( wiced_bt_gatt_write_t *p_data )
{
	uint16_t attr_handle = 	p_data->handle;
	uint8_t  *p_val = 		p_data->p_val;
	uint16_t len = 			p_data->val_len;

	WICED_BT_TRACE("Data: %s, H: %d\n ", p_val ,attr_handle);

    wiced_hal_puart_print(p_val);
    switch(p_val[0])
    {
    case 'S':
    	     process_SOM(p_val);
    	break;

    case 'W':
    	process_Write(p_val);
    	break;



    }
    //wiced_bt_spp_send_session_data(handle, "Data Set: ", 10);
    //wiced_bt_spp_send_session_data(handle, p_data, data_len);


	int i = 0;
    wiced_bool_t validLen = WICED_FALSE;

    wiced_bt_gatt_status_t res = WICED_BT_GATT_INVALID_HANDLE;
    set_data_base();
    // Check for a matching handle entry and find is max available size
//    for (i = 0; i < app_gatt_db_ext_attr_tbl_size; i++)
//    {
//        if (app_gatt_db_ext_attr_tbl[i].handle == attr_handle)
//        {
//            // Detected a matching handle in external lookup table
//            // Verify that size constraints have been met
//            validLen = (app_gatt_db_ext_attr_tbl[i].max_len >= len);
//            if (validLen)
//            {
//                // Value fits within the supplied buffer; copy over the value
//                app_gatt_db_ext_attr_tbl[i].cur_len = len;
//                memcpy(app_gatt_db_ext_attr_tbl[i].p_data, p_val, len);
//                res = WICED_BT_GATT_SUCCESS;
//
//                // Add code for any action required when this attribute is written
//                // For example you may need to write the value into NVRAM if it needs to be persistent
//                switch ( attr_handle )
//                {
//                	case HDLC_STV_CONFIG_VALUE:
//						wiced_hal_gpio_set_pin_output(LED2, !(app_stv_config[0]) );
//						WICED_BT_TRACE( "Turn the LED %s\r\n", app_stv_config[0] ? "ON" : "OFF" );
//						break;
//                }
//            }
//            else
//            {
//                // Value to write does not meet size constraints
//                res = WICED_BT_GATT_INVALID_ATTR_LEN;
//            }
//            break; /* break out of for loop once matching handle is found */
//        }
//    }
    memset(p_val,'\0',64);
    return res;
}

/********************************************************************
 * Function name: set_data_base
 * Description: calibration data conversion. The data is recorded
 *              at the user level and the algorithm
 *              converts it to be able to process it
 ********************************************************************/
void set_data_base(void)
{
	//char    Data_n[]    = { 'A', 'B', 'V', ':', 'o', ' ', '0' };             //Notification Name
	char    Data_n[15];
	uint16_t attr_handle = 	12;


	for(int c=1; c<4; c++)
	{
	data_select[c].value_n=data_rssi_save1[c-1];

	data_select[c].value_d=(data_select[c].value_n%100-data_select[c].value_n%10)/10;
	data_select[c].value_u=data_select[c].value_n%10;

	WICED_BT_TRACE("Data D:%d, U:%d\n ", data_select[1].value_d, data_select[c].value_u );

	data_select[c].value_dt=((data_select[c].value_d)%10)+48;
	data_select[c].value_ut=data_select[c].value_u+48;

	WICED_BT_TRACE("Data_T DT:%d, UT:%d\n ", data_select[c].value_dt, data_select[c].value_ut );
	}




	Data_n[0]='A';
	Data_n[1]=':';
	Data_n[2]=data_select[1].value_dt;
	Data_n[3]=data_select[1].value_ut;
	Data_n[4]=',';
	Data_n[5]='B';
	Data_n[6]=':';
	Data_n[7]=data_select[2].value_dt;
	Data_n[8]=data_select[2].value_ut;
	Data_n[9]=',';
	Data_n[10]='V';
	Data_n[11]=':';
	Data_n[12]=data_select[3].value_dt;
	Data_n[13]=data_select[3].value_ut;


	if(Data_n[2]==48 && Data_n[3]==48)
	{
		Data_n[2]=56;
		Data_n[3]=48;
	}

	if(Data_n[7]==48 && Data_n[8]==48)
	{
		Data_n[7]=56;
		Data_n[8]=53;
	}

	if(Data_n[12]==48 && Data_n[13]==48)
	{
		Data_n[12]=56;
		Data_n[13]=51;
	}


	uint8_t  *p_val = (uint8_t *)Data_n;
	uint16_t len = 			15;


	WICED_BT_TRACE("Data RSSI: %B\n ", p_val );

	int i = 0;
    wiced_bool_t validLen = WICED_FALSE;

    wiced_bt_gatt_status_t res = WICED_BT_GATT_INVALID_HANDLE;

    // Check for a matching handle entry and find is max available size
    for (i = 0; i < app_gatt_db_ext_attr_tbl_size; i++)
    {
        if (app_gatt_db_ext_attr_tbl[i].handle == attr_handle)
        {
            // Detected a matching handle in external lookup table
            // Verify that size constraints have been met
            validLen = (app_gatt_db_ext_attr_tbl[i].max_len >= len);
            if (validLen)
            {
                // Value fits within the supplied buffer; copy over the value
                app_gatt_db_ext_attr_tbl[i].cur_len = len;
                memcpy(app_gatt_db_ext_attr_tbl[i].p_data, p_val, len);
                res = WICED_BT_GATT_SUCCESS;

                // Add code for any action required when this attribute is written
                // For example you may need to write the value into NVRAM if it needs to be persistent
                switch ( attr_handle )
                {
                	case HDLC_STV_CONFIG_VALUE:
						wiced_hal_gpio_set_pin_output(LED2, !(app_stv_config[0]) );
						WICED_BT_TRACE( "Turn the LED %s\r\n", app_stv_config[0] ? "ON" : "OFF" );
						break;
                }
            }
            else
            {
                // Value to write does not meet size constraints
                res = WICED_BT_GATT_INVALID_ATTR_LEN;
            }
            break; /* break out of for loop once matching handle is found */
        }
    }
}
//--------------------------------------------------------------------


/***************************************************************************************
 * This function is invoked when advertisements stop.  Continue advertising if there
 * are no active connections
 ***************************************************************************************/
void beacon_advertisement_stopped(void)
{
    wiced_result_t result;

    // while we are not connected
    if (beacon_conn_id == 0)
    {
        result =  wiced_bt_start_advertisements(BTM_BLE_ADVERT_UNDIRECTED_LOW, 0, NULL);
        WICED_BT_TRACE("wiced_bt_start_advertisements: %d\n", result);
    }
    else
    {
        WICED_BT_TRACE("ADV stop\n");
    }
}

/**************************************************************
 * Setup advertisement data with 16 byte UUID and device name
 **************************************************************/
void beacon_set_app_advertisement_data2(void)
{
    wiced_bt_ble_advert_elem_t adv_elem[2];
    uint8_t num_elem = 0;
    uint8_t flag = BTM_BLE_GENERAL_DISCOVERABLE_FLAG | BTM_BLE_BREDR_NOT_SUPPORTED;

    adv_elem[num_elem].advert_type  = BTM_BLE_ADVERT_TYPE_FLAG;
    adv_elem[num_elem].len          = sizeof(uint8_t);
    adv_elem[num_elem].p_data       = &flag;
    num_elem++;

    adv_elem[num_elem].advert_type  = BTM_BLE_ADVERT_TYPE_NAME_COMPLETE;
    adv_elem[num_elem].len          = strlen((const char *)"LAIRD BLBC");
    adv_elem[num_elem].p_data       = (uint8_t*)"LAIRD BLBC";
    num_elem++;

    wiced_bt_ble_set_raw_advertisement_data(num_elem, adv_elem);
}

/*********************************************************************
 * Function name: beacon_set_app_advertisement_data3
 * Description:   confirmation of boarding event
 *********************************************************************/
void beacon_set_app_advertisement_data3(void)
{
	wiced_result_t         result;
    wiced_bt_ble_advert_elem_t adv_elem[2];
    uint8_t num_elem = 0;
    uint8_t flag = BTM_BLE_GENERAL_DISCOVERABLE_FLAG | BTM_BLE_BREDR_NOT_SUPPORTED;

    adv_elem[num_elem].advert_type  = BTM_BLE_ADVERT_TYPE_FLAG;
    adv_elem[num_elem].len          = sizeof(uint8_t);
    adv_elem[num_elem].p_data       = &flag;
    num_elem++;

    adv_elem[num_elem].advert_type  = BTM_BLE_ADVERT_TYPE_NAME_COMPLETE;
    adv_elem[num_elem].len          = strlen((const char *)"1AIRD BLE");
    adv_elem[num_elem].p_data       = (uint8_t*)"1AIRD BLE";
    num_elem++;

    wiced_bt_ble_set_raw_advertisement_data(num_elem, adv_elem);

    result =  wiced_bt_start_advertisements(BTM_BLE_ADVERT_UNDIRECTED_HIGH, 0, NULL);
    WICED_BT_TRACE("wiced_bt_start_advertisements %d\n", result);
}

/*********************************************************************
 * Function name: gap_transferA
 * Description:   transmits the name of the device type
 *********************************************************************/
void gap_transferA(void)
{
	wiced_result_t         result;
    wiced_bt_ble_advert_elem_t adv_elem[2];
    uint8_t num_elem = 0;
    uint8_t flag = BTM_BLE_GENERAL_DISCOVERABLE_FLAG | BTM_BLE_BREDR_NOT_SUPPORTED;

    adv_elem[num_elem].advert_type  = BTM_BLE_ADVERT_TYPE_FLAG;
    adv_elem[num_elem].len          = sizeof(uint8_t);
    adv_elem[num_elem].p_data       = &flag;
    num_elem++;

    adv_elem[num_elem].advert_type  = BTM_BLE_ADVERT_TYPE_NAME_COMPLETE;
    adv_elem[num_elem].len          = strlen((const char *)"LAIRD BLBC");
    adv_elem[num_elem].p_data       = (uint8_t*)"LAIRD BLBC";
    num_elem++;

    wiced_bt_ble_set_raw_advertisement_data(num_elem, adv_elem);

    result =  wiced_bt_start_advertisements(BTM_BLE_ADVERT_UNDIRECTED_HIGH, 0, NULL);
    WICED_BT_TRACE("wiced_bt_start_advertisements %d\n", result);
}


/********************************************************
 * Function name: gap_transferB
 * Description: boarding notification message
 ********************************************************/
void gap_transferB(void)
{
	wiced_result_t         result;
    wiced_bt_ble_advert_elem_t adv_elem[2];
    uint8_t num_elem = 0;
    uint8_t flag = BTM_BLE_GENERAL_DISCOVERABLE_FLAG | BTM_BLE_BREDR_NOT_SUPPORTED;

    adv_elem[num_elem].advert_type  = BTM_BLE_ADVERT_TYPE_FLAG;
    adv_elem[num_elem].len          = sizeof(uint8_t);
    adv_elem[num_elem].p_data       = &flag;
    num_elem++;

    adv_elem[num_elem].advert_type  = BTM_BLE_ADVERT_TYPE_NAME_COMPLETE;
    adv_elem[num_elem].len          = strlen((const char *)"L41RD BLE");
    adv_elem[num_elem].p_data       = (uint8_t*)"L41RD BLE";
    num_elem++;

    wiced_bt_ble_set_raw_advertisement_data(num_elem, adv_elem);

    result =  wiced_bt_start_advertisements(BTM_BLE_ADVERT_UNDIRECTED_HIGH, 0, NULL);
    WICED_BT_TRACE("wiced_bt_start_advertisements %d\n", result);
}

/********************************************************
 * Function name: gap_transferER
 * Description: link message with external reader
 ********************************************************/
void gap_transferER(void)
{
	wiced_result_t         result;
    wiced_bt_ble_advert_elem_t adv_elem[2];
    uint8_t num_elem = 0;
    uint8_t flag = BTM_BLE_GENERAL_DISCOVERABLE_FLAG | BTM_BLE_BREDR_NOT_SUPPORTED;

    adv_elem[num_elem].advert_type  = BTM_BLE_ADVERT_TYPE_FLAG;
    adv_elem[num_elem].len          = sizeof(uint8_t);
    adv_elem[num_elem].p_data       = &flag;
    num_elem++;

    adv_elem[num_elem].advert_type  = BTM_BLE_ADVERT_TYPE_NAME_COMPLETE;
    adv_elem[num_elem].len          = strlen((const char *)"LA1RO BLE");
    adv_elem[num_elem].p_data       = (uint8_t*)"LA1RO BLE";
    num_elem++;

    wiced_bt_ble_set_raw_advertisement_data(num_elem, adv_elem);

    result =  wiced_bt_start_advertisements(BTM_BLE_ADVERT_UNDIRECTED_HIGH, 0, NULL);
    WICED_BT_TRACE("wiced_bt_start_advertisements %d\n", result);
}

/*********************************************************************
 * Function name: gap_cfb
 * Description:   sends the addressed devices to the reader
 * @param[in]     data_cfb: event number
 *********************************************************************/
void gap_cfb(char *data_cfb)
{
	memcpy(datac_cfb,data_cfb,6);
	memcpy(dataa_cfb2,datafv_cfb,8);
	memcpy(dataa_cfb2 + sizeof(datafv_cfb), &datac_cfb, sizeof(datac_cfb));
	WICED_BT_TRACE_ARRAY(dataa_cfb2, 18, "BUFFER Datav AAA");

	wiced_result_t         result;
    wiced_bt_ble_advert_elem_t adv_elem[2];
    uint8_t num_elem = 0;
    uint8_t flag = BTM_BLE_GENERAL_DISCOVERABLE_FLAG | BTM_BLE_BREDR_NOT_SUPPORTED;

    adv_elem[num_elem].advert_type  = BTM_BLE_ADVERT_TYPE_FLAG;
    adv_elem[num_elem].len          = sizeof(uint8_t);
    adv_elem[num_elem].p_data       = &flag;
    num_elem++;

    adv_elem[num_elem].advert_type  = BTM_BLE_ADVERT_TYPE_NAME_COMPLETE;
    adv_elem[num_elem].len          = strlen((const char *)dataa_cfb2);
    adv_elem[num_elem].p_data       = (uint8_t*)dataa_cfb2;
    num_elem++;

    wiced_bt_ble_set_raw_advertisement_data(num_elem, adv_elem);

    result =  wiced_bt_start_advertisements(BTM_BLE_ADVERT_UNDIRECTED_HIGH, 0, NULL);
    WICED_BT_TRACE("wiced_bt_start_advertisements datb %d\n", result);
}

/*******************************************************************
 * Function name: gap_cfbV
 * Description: transmitting messages from addressed devices
 *******************************************************************/
void gap_cfbV(char *data_cfbV)
{
	memcpy(datac_cfb,data_cfbV,6);
	memcpy(dataa_cfb2,datafv_cfbV,8);
	memcpy(dataa_cfb2 + sizeof(datafv_cfbV), &datac_cfb, sizeof(datac_cfb));
	WICED_BT_TRACE_ARRAY(dataa_cfb2, 18, "BUFFER Datav AAA");

	wiced_result_t         result;
    wiced_bt_ble_advert_elem_t adv_elem[2];
    uint8_t num_elem = 0;
    uint8_t flag = BTM_BLE_GENERAL_DISCOVERABLE_FLAG | BTM_BLE_BREDR_NOT_SUPPORTED;

    adv_elem[num_elem].advert_type  = BTM_BLE_ADVERT_TYPE_FLAG;
    adv_elem[num_elem].len          = sizeof(uint8_t);
    adv_elem[num_elem].p_data       = &flag;
    num_elem++;

    adv_elem[num_elem].advert_type  = BTM_BLE_ADVERT_TYPE_NAME_COMPLETE;
    adv_elem[num_elem].len          = strlen((const char *)dataa_cfb2);
    adv_elem[num_elem].p_data       = (uint8_t*)dataa_cfb2;
    num_elem++;

    wiced_bt_ble_set_raw_advertisement_data(num_elem, adv_elem);

    result =  wiced_bt_start_advertisements(BTM_BLE_ADVERT_UNDIRECTED_HIGH, 0, NULL);
    WICED_BT_TRACE("wiced_bt_start_advertisements datb %d\n", result);
}

extern void process_Write(uint8_t *data_Write);
extern void process_CMA(uint8_t *data_C_MA);
extern void process_data_config(uint8_t *data_dc);
extern void process_ODT(uint8_t *data_ODT);
extern void process_SOM(uint8_t *data_S_OM);


uint8_t Uart_BuffRX[64];
int cont_buffRX=0;

/********************************************************
 * Function name: rx_cback
 * Description: data reception through uart
 * @parameter data: variable with each input byte
 ********************************************************/
void rx_cback( void *data )
{
    uint8_t  readbyte;
    uint8_t *dat=NULL;
    wiced_result_t            result;
    uint8_t                   buffer[(6 * 2) + 16];
    wiced_bt_mesh_core_init_t init = { 0 };



    /* Read one byte from the buffer and (unlike GPIO) reset the interrupt */
    wiced_hal_puart_read( &readbyte );
    wiced_hal_puart_reset_puart_interrupt();

    dat=&readbyte;
    Uart_BuffRX[cont_buffRX] =dat[0];
    cont_buffRX++;
    if(readbyte=='\n')
    {
    cont_buffRX=0;
    //wiced_hal_puart_print(Uart_BuffRX);
    switch(Uart_BuffRX[0])
    {
    case 'W':
    	    process_Write(Uart_BuffRX);
    break;

    case 'C':
    	    process_CMA(Uart_BuffRX);
    break;

    case 'R':
    	    process_data_config(Uart_BuffRX);
    break;

    case 'O':
    	     process_ODT(Uart_BuffRX);
    	break;

    case 'S':
    	     process_SOM(Uart_BuffRX);
    	break;

    }
    memset(Uart_BuffRX,'\0',64);
    }


}

