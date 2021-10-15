#include<pantallas.h>
#include <reg51.h>
#include <string.h>

sbit sel_com = P0^7;				//Micro switch	
/* varibles externas*/
extern  unsigned char Tipo_Vehiculo;	
extern unsigned char 	Raspberry;

/*funciones externas*/
extern char putchar (char c);
extern void Block_read_Clock(unsigned char *datos_clock);
extern void DebugBufferMF(unsigned char *str,unsigned char num_char,char io);
extern void Debug_Dividir_texto();
extern void clean_tx();
//extern void serie_ascii_siceros_l(unsigned char *serie);
extern void clear_buffer();
extern void Delay_20us(unsigned int cnt);

#define ERROR_LOOP							0XE0
#define TARJETA_INVALIDA				0XE1
#define TARJETA_SIN_FORMATO	    0xDF
#define ERROR_COD_PARK					0XE5
#define SIN_INGRESO							0XE6
#define NO_REGISTRA							0x04
#define EXCEDE_HORARIO					0x07
#define UN_MOMENTO							0x09
#define EXCEDE_GRACIA						0XE8
#define FUERA_DE_LINEA					0XB6
#define LECTURA_WIEGAND					0xB0
#define LECTURA_DE_TARJETAS			0xb0
#define NO_PAGO									0XE7

#define INGRESO									0xFA
#define BIENVENIDO							0XFE
#define GRACIAS									0XFF

#define NOTIFIQUE_EVP						'N'
#define NO_NOTIFIQUE_EVP				'S'
#define COMPARANDO_PLACA				'P'						/*msj de error de placa*/
#define ENVIANDO_COD						'D'
#define INFO1										'I'
#define INFO2										'i'


#define NUL											0x00
#define SOH											0x01
#define STX											0x02

#define CAN											0X18


/*------------------------------------------------------------------------------
transmite el caracter pto serie
data_com = al caracter a escribir
enable_char_add = si esta en (1) envia un null (0) adicional, si es (0) no envia caracter adicional
------------------------------------------------------------------------------*/
void tx_chrlcd (unsigned char data_com, unsigned char enable_char_add)
{
	unsigned char d;
	
	d=putchar(data_com);
	if (enable_char_add != 0)	
		{d=putchar(0x00);}

		
}
/*------------------------------------------------------------------------------
envia un msj asta null(0)
msg= apuntador del msj
enable_char_add = si esta en (1) envia un null. (0) adicional, si es (0) no envia caracter adicional
------------------------------------------------------------------------------*/
void LCD_txt (unsigned char * msg,unsigned char enable_char_add )
{
	unsigned char i;
	
	for (i=0; msg[i] != '\0'; i++)
	{
 	 	tx_chrlcd(msg[i],enable_char_add);
	}
}
/*------------------------------------------------------------------------------
Escribo el reloj en ascii en bloque 
msg= apuntador del msj
length_char longitud de la tram
enable_char_add = si esta en (1) envia un null (0) adicional, si es (0) no envia caracter adicional
------------------------------------------------------------------------------*/
void LCD_txt_num_char(unsigned char * msg,unsigned char length_char, unsigned char enable_char_add)
{
	unsigned char i;
	 
	for (i=0; i<length_char; i++)
	{
 	 	tx_chrlcd(msg[i],enable_char_add);
	}
}
/*------------------------------------------------------------------------------
Escribo el reloj en ascii en bloque 
AA 80 28 trama de inicio de configuracion de la pantalla
07 numero de caracteres de la trama de reloj
20 19 03 26 09 21 20  el dato del reloj
------------------------------------------------------------------------------*/
void Reloj_Pantalla_Lcd()
{

 unsigned char Ini_Clock_LCD   []={0xaa,0x80,0x28,0x07,0x20,0x00,0,0,0,0,20,0,0} ;
					sel_com=0;																																			/*switch del pto serie a la pantalla*/
					Block_read_Clock(Ini_Clock_LCD+5);																							/*Leo el reloj programado*/
					//Debug_Dividir_texto();																													/*lineas de separacion del texto*/
					//DebugBufferMF(Ini_Clock_LCD,12,0);																							/*muestra la trama por debug*/
					//Debug_Dividir_texto();																													/*linea de separacion de texto*/
					REN = 0;																																				/*inhabilita recepcion de datos*/
					LCD_txt_num_char(Ini_Clock_LCD,13,0);																						/*cmd de inicializacion del reloj del lcd*/
																											
					REN = 1;																																				/*habilita recepcion de datos*/
					sel_com=1;																																			/*switch pto serie a verificador o expedidor */
}
void Raspberry_data (unsigned char *msjpantalla)
{
	
	unsigned char i;
	unsigned char lenth_cadena;
	unsigned char d;
	
	
	lenth_cadena=strlen(msjpantalla);
	
	for (i=0; i < lenth_cadena ; i++)
	{
			
	
		d=putchar(*(msjpantalla + i));
		for (d=0; d<200; d++)
   {
	 }

	}
		

	
}
/*------------------------------------------------------------------------------
Rutina de msj de pantalla
0xaa,0x80,0x18 cmd de inicio lcd
0x01	numero de linea
0x02  tiempo de duracion del msj en minutos si especifica (00)no hay msj en el lcd si es (ff) el msj constante
0x00,0xnn numero de caracteres a impirmir
------------------------------------------------------------------------------*/
void PantallaLCD(unsigned char cod_msg)
{
//unsigned char bar[15];
unsigned char datos[40];
//unsigned char Ini_LCD_Line_one   []={0xaa,0x80,CAN,SOH,STX,0x00} ;
//unsigned char Ini_LCD_Line_two   []={0xaa,0x80,0x18,0x02,0x02,0x00} ;

	
//unsigned char num_chr;

	
		sel_com=0;
	
		if (Raspberry==0)
		{ 
			
			//LCD_txt (Ini_LCD_Line_one,0);
			
			switch (cod_msg)
			{
				/*
				case 'P':
					
					num_chr=strlen((unsigned char *) "ERROR: VALIDANDO PLACA... ");
		   		tx_chrlcd(0x00,0);
					tx_chrlcd(num_chr*2,0);
					LCD_txt ((unsigned char *)       "ERROR: VALIDANDO PLACA... ",1);
					
					break;

				case ERROR_LOOP:
					
					num_chr=strlen((unsigned char *) "ERROR: LOOP1 SIN PRESENCIA VEHICULAR ");	
					tx_chrlcd(0x00,0);
					tx_chrlcd(num_chr*2,0);
					LCD_txt ((unsigned char *)       "ERROR: LOOP1 SIN PRESENCIA VEHICULAR ",1);
					
					break;
				
				case TARJETA_INVALIDA:
					
					num_chr=strlen((unsigned char *) "ERROR: TARJETA INVALIDA ");	
					tx_chrlcd(0x00,0);
					tx_chrlcd(num_chr*2,0);
					LCD_txt ((unsigned char *)       "ERROR: TARJETA INVALIDA ",1);
					
					break;
				
				case ERROR_COD_PARK:
		
					num_chr=strlen((unsigned char *) "TARJETA NO ES DEL PARQ. ");
					tx_chrlcd(0x00,0);
					tx_chrlcd(num_chr*2,0);
					LCD_txt ((unsigned char *)       "TARJETA NO ES DEL PARQ. ",1);
		
					break;
					
				case TARJETA_SIN_FORMATO:
		
					num_chr=strlen((unsigned char *) "TARJETA SIN FORMATO ");
					tx_chrlcd(0x00,0);
					tx_chrlcd(num_chr*2,0);
					LCD_txt((unsigned char *)        "TARJETA SIN FORMATO ",1);

				  break;
					
				case SIN_PAGO:
		
					num_chr=strlen((unsigned char *) "TARJETA NO REGISTRA PAGO ");
					tx_chrlcd(0x00,0);
					tx_chrlcd(num_chr*2,0);
					LCD_txt ((unsigned char *)       "TARJETA NO REGISTRA PAGO ",1);

					break;
						
				case EXCEDE_GRACIA:
		
					num_chr=strlen((unsigned char *) "EXCEDE TIEMPO DE GRACIA ");
					tx_chrlcd(0x00,0);
					tx_chrlcd(num_chr*2,0);
					LCD_txt ((unsigned char *)       "EXCEDE TIEMPO DE GRACIA ",1);
			
					break;
				
				case 	FUERA_DE_LINEA:
					
					num_chr=strlen((unsigned char *) "FUERA DE LINEA ");
					tx_chrlcd(0x00,0);
					tx_chrlcd(num_chr*2,0);
					LCD_txt((unsigned char *)        "FUERA DE LINEA ",1);
				
					break;
				
				case	BIENVENIDO:
					
					num_chr=strlen((unsigned char *) "ACERQUE SU TICKET ");
					tx_chrlcd(0x00,0);
					tx_chrlcd(num_chr*2,0);
					LCD_txt((unsigned char *)        "ACERQUE SU TICKET ",1);
					
					break;
				
				case GRACIAS:
				  num_chr=strlen((unsigned char *) "GRACIAS ");
					tx_chrlcd(0x00,0);
					tx_chrlcd(num_chr*2,0);
					LCD_txt((unsigned char *)        "GRACIAS ",1);
					
					break;
				case	SIN_INGRESO:
					
					num_chr=strlen((unsigned char *) "ERROR: SIN INGRESO ");
					tx_chrlcd(0x00,0);
					tx_chrlcd(num_chr*2,0);
					LCD_txt((unsigned char *)        "ERROR: SIN INGRESO ",1);
					
					break;
				case INGRESO:
					serie_ascii_siceros_l(bar);
				 	strcpy(datos,"TICKET No.");
				  strcat(datos,bar);
				
					num_chr=strlen(datos)+1;
				 
				
					tx_chrlcd(0x00,0);
					tx_chrlcd(num_chr*2,0);
				  LCD_txt_num_char(datos,num_chr,1);
				 								
					break;
					*/
	}
				sel_com=1;	
	}
		else
		{
			sel_com=0;   
         switch (cod_msg)
         {
					 case BIENVENIDO:
						strcpy(datos,"a;03;ACERQUE SU TICKET\n\0");
						Raspberry_data (datos);
            break;
					 	case GRACIAS:
						strcpy(datos,"a;91;GRACIAS ..... \n\0");
						Raspberry_data (datos);
            break;
						case NO_REGISTRA:
						strcpy(datos,"a;87;No TIKET NO REGISTRADO \n\0");
						Raspberry_data (datos);
            break;
						case EXCEDE_HORARIO:
						strcpy(datos,"a;87;EXCEDE HORARIO...\n\0");
						Raspberry_data (datos);
						
						case UN_MOMENTO:
						strcpy(datos,"a;87;UN MOMENTO .... \n\0");
						Raspberry_data (datos);
						break;
						case ERROR_LOOP:
						strcpy(datos,"a;87;SIN PRESENCIA VEHICULAR.... \n\0");
						Raspberry_data (datos);
						break;
						
						case NO_PAGO:
						strcpy(datos,"a;87;NO REGISTRA PAGO.... \n\0");
						Raspberry_data (datos);
						break;
						default:
						break;
						
				 }
			 sel_com=1; 	 
		}
}
void PantallaLCD_LINEA_2(unsigned char cod_msg, unsigned char *buffer)
{
	
unsigned char Ini_LCD_Line_one   []={0xaa,0x80,0x18,0x01,0x02,0x00} ;
unsigned char datos[40];

unsigned char num_chr;
	
sel_com=0;
	
		if (Raspberry==0)
		{
		
			
			switch (cod_msg)
			{
		
				case	LECTURA_DE_TARJETAS:
					LCD_txt (Ini_LCD_Line_one,0);
					num_chr=strlen((unsigned char *) "WIEGAND ");
					num_chr=num_chr+strlen(buffer)+1;
					tx_chrlcd(0x00,0);
					tx_chrlcd(num_chr*2,0);
					LCD_txt((unsigned char *)        "WIEGAND ",1);	 											/*funcion q trasmite el msj al LCD  y el (1) coloca los caracteres NULL*/
					LCD_txt(buffer,1);	 																									/*funcion q trasmite el msj al LCD  y el (1) coloca los caracteres NULL*/
					
				
				
				
					break;
				
				
			}
				sel_com=1;	
	}
		else 
		{
			 sel_com=0;  
			
       switch (cod_msg)
			 {
				  case GRACIAS:
            strcpy(datos,"a;91;GRACIAS ");
						num_chr=strlen(buffer);
						*(buffer+(num_chr-1))=0;
						strcat(datos,buffer);
						strcat(datos,"\n\0");
						Raspberry_data (datos);
					
            break;
					case	LECTURA_WIEGAND:
						
										 
						strcpy(datos,"a;92;WIEGAND ");
								 
						strcat(datos,buffer);
						
						strcat(datos,"\n\0");
						
				 
						Raspberry_data (datos);
						
					
						break;
				 
			 }
			 sel_com=1;	
		}
}
