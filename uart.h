/*********************************************
Diseñado Ing. Jaime Pedraza
Marzo 05 de 2019
*********************************************/
/*
@file		uart.h
@brief	header file for delay.c
*/

#ifndef _UART_H_
#define _UART_H_
/*funciones prototipo*/
void com_initialize (void);
void com_baudrate ();
char com_putchar (unsigned char c);
int com_getchar (void);
unsigned char com_rbuflen (void);
unsigned char com_tbuflen (void);
void clear_buffer();
void DebugBuffer_Uart(unsigned char num_char);
void Debug_chr_uart(unsigned char Dat);
/*funciones externas*/



#endif	/* _UART_H_ */



