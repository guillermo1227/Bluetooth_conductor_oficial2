/*
 * localization.c
 *
 *  Created on: 21 ago 2024
 *      Author: Usuario1
 */
#include "localization.h"

void init_Acarreo_parameters(void)
{
	Data_BecLoc.state_flag_B = No_Beacon;
	Data_BecLoc.state_flagName = No_Name;
}

void Put_in_beacon(uint8_t *P_punteroURL,wiced_bt_ble_scan_results_t *p_scan_result)
{
	uint8_t * P_url = &P_punteroURL[0];
	P_punteroURL = &P_punteroURL[4];
	//WICED_BT_TRACE("Info de URL:%B\n",P_punteroURL);
	if(Data_BecLoc.state_flag_B == No_Beacon)  /* Save information of the Beacon */
	{
		if( !memcmp( P_punteroURL, Filt_UUID1, sizeof( Filt_UUID1 )) )
		{
			//WICED_BT_TRACE("------------->Guardo infromacion Beacon Bascula\n");
			memcpy(Data_BecLoc.macG,p_scan_result->remote_bd_addr,6);
			Data_BecLoc.state_flag_B = In_Beacon;
			Data_BecLoc.type_G = bascula;
		}
		else if( !memcmp( P_punteroURL, Filt_UUID2, sizeof( Filt_UUID2 )) )
		{
			//WICED_BT_TRACE("------------->Guardo infromacion Beacon Carga\n");
			memcpy(Data_BecLoc.macG,p_scan_result->remote_bd_addr,6);
			Data_BecLoc.state_flag_B = In_Beacon;
			Data_BecLoc.type_G = carga;
			//WICED_BT_TRACE("Mac guardada%B\n",Data_BecLoc.macG);
		}
		else if( !memcmp( P_punteroURL, Filt_UUID3, sizeof( Filt_UUID3 )) )
		{
			//WICED_BT_TRACE("------------->Guardo infromacion Beacon Descarga\n");
			memcpy(Data_BecLoc.macG,p_scan_result->remote_bd_addr,6);
			Data_BecLoc.state_flag_B = In_Beacon;
			Data_BecLoc.type_G = descarga;
		}
	}

	memcpy(scanner_url,&P_url[5],7);
	if(memcmp("BNCarga",scanner_url,7)==0 && Data_BecLoc.state_flagName == No_Name && Data_BecLoc.type_G == carga)
	{
		//WICED_BT_TRACE("*********************************Nombre BNCarga\n");
		//WICED_BT_TRACE("Type: %d\n",Data_BecLoc.type_G);
		Data_BecLoc.state_flagName = In_Name;
		memcpy(Data_BecLoc.nameB,"BNCarga",7); /* BNCarga */
		Data_BecLoc.nameB[7] = '\0';
	}

	if(Data_BecLoc.state_flag_B == In_Beacon || Data_BecLoc.state_flagName == In_Name)  /* Empieza a transmitirr informacion */
	{
		//WICED_BT_TRACE("---------->Iniciar el timer\n");
		if(Data_BecLoc.state_flag_B == In_Beacon)
			Data_BecLoc.state_flag_B = Transmit_Beacon;
		else
			Data_BecLoc.state_flagName = Transmit_Name;

		restart_timer();
	}

	if((memcmp(p_scan_result->remote_bd_addr,Data_BecLoc.macG,6)==0 && (Data_BecLoc.state_flag_B == Transmit_Beacon)) ||
			(memcmp("BNCarga",scanner_url,7) == 0 && Data_BecLoc.state_flagName == Transmit_Name)){		/* Sacar el beacon cuando se deja de ver */
		Data_BecLoc.out_beacon=found_data;
	}
}
