#include <num_serie.h>

/*concecutivo de la factura*/
#define EE_CONSECUTIVO3	0X0500


/*funciones prototipo*/

extern void LeerMemoria(unsigned int addres,unsigned char *res);
extern void EscribirMemoria(unsigned char control,unsigned int  addres,unsigned char  *res);
extern long   atol (const char *s1);
extern int sprintf  (char *, const char *, ...);
/*---------------------------------------------------------------------------------------------------------------------------------------
escribe el numero consecutivo del recibo en eeprom 
el dato llega en ascii y se convierte a hex y se almacena en eeprom
----------------------------------------------------------------------------------------------------------------------------------------*/

void graba_serie_ascii_hex(unsigned char *buffer)
{
	
		
		*(buffer+10)=0;
	  EscribirMemoria(0xa8,EE_CONSECUTIVO3,buffer);
}
/*---------------------------------------------------------------------------------------------------------------------------------------
se recibe el dato en hex en un long  yse transforma en un arreglo
en  hex y se escribe en eeprom
----------------------------------------------------------------------------------------------------------------------------------------*/
/*
void graba_serie(unsigned long int numero)
{
		unsigned char cod_serie[10];
		sprintf(cod_serie,"%lu",numero);
		EscribirMemoria(0xa8,EE_CONSECUTIVO3,cod_serie);
}
*/
/*---------------------------------------------------------------------------------------------------------------------------------------
leo la serie de eeprom y la debuelvo en 
un long int en hex
----------------------------------------------------------------------------------------------------------------------------------------*/

unsigned long int  Leer_serie ()
{
	unsigned long int numero;
	unsigned char cod_serie[10];
	
	LeerMemoria(EE_CONSECUTIVO3,cod_serie);
	return numero= atol(cod_serie);
	
}	
/*---------------------------------------------------------------------------------------------------------------------------------------
incremento el numero de serie y lo grabo en eeprom
----------------------------------------------------------------------------------------------------------------------------------------*/
/*
void incremente_serie()
{
	long int numero;
 
	numero=Leer_serie ();										/*leo la serie de eeprom*/
/*	numero++;
 	
	graba_serie(numero);
	
	
}
*/
/*---------------------------------------------------------------------------------------------------------------------------------------
rutina que quita los ceros a la izquierda en ascii 0x30 y los cambia  por cero
al primer caracter diferente de cero ascii 0x30 aborta la funcion
----------------------------------------------------------------------------------------------------------------------------------------*/

void ceros_izq(unsigned char *dato)
{
	unsigned char p,j,temp;

			p=9;
			for (j=0; j<10; j++)
			{
		 	 	temp=*(dato+(p-j));
				if (temp==0x30)
				{
				 *(dato+(p-j))=0;
				}
				else
				{
				 	break;
				}	
			}
}	
/*---------------------------------------------------------------------------------------------------------------------------------------
leo la serie en eeprom en hex y lo paso a ascii
y elimino los ceros a la izquierda
----------------------------------------------------------------------------------------------------------------------------------------*/

void serie_ascii_siceros_l(unsigned char *serie)
{
	unsigned long int numero;
	

	numero=Leer_serie ();
	sprintf(serie,"%lu",numero);
	ceros_izq(serie);

}