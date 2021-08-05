/*********************************************
jp
/*********************************************/
/*
@file		Accescan.h
@brief	header file for Accescan.c
*/

#ifndef _MONITOR_H_
#define _MONITOR_H_


unsigned char recibe_cmd_Monitor(unsigned char *buffer);
void Valida_Trama_Monitor(unsigned char *buffer, unsigned char length_trama);

void Monitor_chr (unsigned char *str,unsigned char num_char);
void Cmd_LPR_Salida_wiegand(unsigned char *buffer);
void Cmd_LPR_Salida(unsigned char *buffer_S1_B0,unsigned char *buffer_S1_B2);
void Cmd_Monitor(unsigned char cmd);
void clear_placa();
void Cmd_LPR_Salida_print(unsigned char *msj,unsigned char vehiculo);
#endif	/*_MONITOR_H_ */
