
/*********************************************
jp
*********************************************/
/*
@file		num_serie.h
@brief	header file for eeprom.c
*/

#ifndef _NUM_SERIE_H_
#define _NUM_SERIE_H_

/*funciones prototipo*/




void graba_serie_ascii_hex(unsigned char *buffer);
void graba_serie(unsigned long int numero);
unsigned long int  Leer_serie ();
void incremente_serie();
void serie_ascii_siceros_l(unsigned char *serie);
void ceros_izq(unsigned char *dato);
#endif	/* _NUM_SERIE_H_ */