/*********************************************
jp
*********************************************/
/*
@file		printer_nano.h
@brief	header file for printer_nano.c
*/

#ifndef _PRINTER_NANO_H_
#define _PRINTER_NANO_H_


void Inicializa_Print(void);
void Posicion_tabulacion(unsigned char n, unsigned char k);
void Move_car(void);
void Justify(unsigned char dato_print);
void JustificaIzq(void);
void Centrado(void);
void JustificaDrc(void);
void Status(unsigned char StatusConsulta);
void Bold(unsigned char negrilla);
void Tama_letra(unsigned char tama);
void Alimenta_nline(unsigned char nline);
void Alimentar_paper(unsigned char nline);
void Espcs_ent_chr(unsigned char espacios);
void FullCut(void);
void Print_datos_cliente();
void height_code_bar(unsigned char height);
void position_Hri(unsigned char position);
void Fount_char_hri();
void Prin_cod_barra(unsigned char tipo,unsigned char *bar);
void Print_cod_barras();
void Print_msj_cod_barras();
void print_ticket();
void Interlineado(unsigned char n);
void Genera_ticket(void);

void Print_msj_cod_barras();
void Funcion_165();
void Funcion_169();
void Funcion_167();
void Funcion_180(unsigned char *datos);
void Funcion_181();
void Print_datos_cliente_QR();
void Print_cod_QR();
#endif	/* _PRINTER_NANO_H_ */