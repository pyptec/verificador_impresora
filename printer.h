/*********************************************
jp
*********************************************/
/*
@file		printer.h
@brief	header file for printer.c
*/

#ifndef _PRINTER_H_
#define _PRINTER_H_


void Inicializa_Print(void);
void Justify(unsigned char dato_print);
void Posicion_tabulacion(void);
void Move_car(void);
void Status(unsigned char StatusConsulta);
void MargenIzq(void);
void Bold(void);
void JustificaIzq(void);
void Centrado(void);
void JustificaDrc(void);
void Tamaño_Char(unsigned char tamaño);
void Alimenta_nline(unsigned char nline);
void Alimentar_paper(unsigned char nline);
void Espcs_ent_chr(unsigned char espacios);
void QR_Size(void);
void FullCut(void);
void Print_ticket(void);

#endif	/* _PRINTER_H_ */