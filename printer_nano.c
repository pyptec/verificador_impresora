#include <printer_nano.h>
#include <reg51.h>
#include <string.h>
/*comandos de impresora*/
#define NUL	0x00
#define ETX	0x03
#define EOT	0x04
#define ENQ	0x05
#define ACK	0x06
#define BEL	0X07
#define CR	0x0d
#define LF	0x0a
#define DLE	0x10
#define ESC	0x1B
#define GS	0x1D
#define SP	0x20
#define DC2	0x12
#define CODE39	0X04
#define	CODE128	0X73

/*direcciones de memoria de los datos de la factura*/

#define	EE_CLIENTE						0X0010
#define EE_NIT								0x0040
#define EE_PARQUEADERO				0X0050
#define EE_ADDRES							0x0080
#define EE_MSJ1								0X00B0
#define EE_MSJ2								0X00f1
#define EE_MSJ3								0X0132
#define EE_MSJ4								0X0173
#define EE_MSJ5								0X01b4
#define EE_MSJ6								0X01f5
#define EE_MSJ7								0X0236
/*----------------------------------------------------------------------------
Definiciones de sequencias de impresion de ticket
------------------------------------------------------------------------------*/

#define SEQ_INICIO						0X00	
#define SEQ_QUEST_PRINT				0X01
#define SEQ_RTA_QUEST					0x02
#define SEQ_PRINT							0x03
#define SEQ_RELEASE						0x04
/*----------------------------------------------------------------------------
msj de lcd tarjeta y lcd serie
------------------------------------------------------------------------------*/

#define ERROR_LOOP							0XE0

//#define SEQ_CHECK_STATUS			0X02
/*----------------------------------------------------------------------------
tiempo de delay entre funciones
------------------------------------------------------------------------------*/

#define 	TIME_CARD					20		//50
#define 	TIME_RX						70		//
#define 	TIME_PLACA				55
#define 	TIME_PULSADOR			10

/*----------------------------------------------------------------------------
definicion de recepcion serial 
------------------------------------------------------------------------------*/

#define  ESPERA_RX 					0  					//espera el primer cmd de recepcion del verificado 


/*---------------------------------------------------------------------------------
definiciones de la pantalla
-----------------------------------------------------------------------------------*/
#define INGRESO									0xFA
#define BIENVENIDO							0XFE
#define AUDIO_IN							  0XA0
#define AUDIO_ENTER				      0XA1
/*variables externas*/

extern unsigned char g_cEstadoComSoft;
extern unsigned char g_cEstadoImpresion;
extern unsigned char ValTimeOutCom;
extern unsigned char buffer_ready;
extern unsigned char Timer_wait;
extern idata unsigned char  Buffer_Rta_Lintech[];
extern  unsigned char placa[];
extern  unsigned char Tipo_Vehiculo;	
extern	unsigned char QR_BARRAS;

/*funciones prototipo*/

extern char putchar (char);

extern void LeerMemoria(unsigned int addres,unsigned char *res);
extern int printf   (const char *, ...);
extern void Block_read_clock_ascii(unsigned char *datos_clock);
extern void Debug_txt_Tibbo(unsigned char * str);
extern unsigned char  ValidaSensoresPaso(void);
extern void sel_Pulsa(void);
extern void send_portERR(unsigned char cod_err);
extern void PantallaLCD(unsigned char cod_msg);
extern void sel_Sensor2(void);
extern void Cmd_Monitor(unsigned char cmd);
extern void incremente_serie();
extern void Trama_pto_paralelo_printer(unsigned char cmd);
extern void DebugBufferMF(unsigned char *str,unsigned char num_char,char io);
extern void clear_buffer();
extern void serie_ascii_siceros_l(unsigned char *serie);
extern void clear_placa();

/*pines del micro*/

sbit DataIn = P1^1;		
sbit lock = P1^7;						//Relevo 	
/*externo bit*/
extern bit placa_ready;
//extern bit buffer_ready;
/*------------------------------------------------
Cmd de inicio de impresora
------------------------------------------------*/
void Inicializa_Print(void)
{
	putchar(ESC);
	putchar('@');
}
/*-----------------------------------------------
establece posisicon de tabulacion horizontal
n=1 - 255 // establece el conjunto de fuentes determinado 0x35
k= 0 - 32	//indica el numero de posisicon de tabulacion
------------------------------------------------*/
void Posicion_tabulacion(unsigned char n, unsigned char k)
{
	putchar(ESC);
	putchar('D');
	putchar(n);
	putchar(k);
}
/*-----------------------------------------------

------------------------------------------------*/
void Move_car()
{
	putchar(GS);
	putchar('P');
	putchar(0xB4);
	putchar(0XB4);
}
/*------------------------------------------------
Cmd de justificado del texto
------------------------------------------------*/
void Justify(unsigned char dato_print)
{
	switch (dato_print)
	{

			case 'L':
				JustificaIzq();
			break;
			case 'C':
				Centrado();
			break;
			case 'R':
				JustificaDrc();
			break;
		
			default:
			
			break;
		}
	
}
/*-----------------------------------------------------------------------------------------------
posicion del texto
(C)= 0 centrado
(l)= 1 justificado a la izquierda
(2)= 2	Justificado a la derecha
-------------------------------------------------------------------------------------------------*/	

void JustificaIzq(void)
{
	putchar(ESC);
	putchar('a');	
	putchar('0');
}

void Centrado(void)
{
	putchar(ESC);
	putchar('a');	
	putchar('1');
}

void JustificaDrc(void)
{
	putchar(ESC);
	putchar('a');	
	putchar('2');
}
/*------------------------------------------------
Cmd del tipo de fuente o letra
'0'
'1' TIPO DE LETRA PEQUEÑO
'2'
'3'
'4'
------------------------------------------------*/
void Tipo_fuente(unsigned char fuente)
{
	putchar(ESC);
	putchar('M');	
	putchar(fuente);
}	

/*------------------------------------------------
Cmd que separa un renglon del otro N lineas 
------------------------------------------------*/
void Interlineado(unsigned char n)
{
	putchar(ESC);
	putchar('3');
	putchar(n);	
}
/*------------------------------------------------
Cmd que pone negrilla
00= normal
01=negrilla
------------------------------------------------*/
void Bold(unsigned char negrilla)
{
	putchar(ESC);
	putchar('E');	
	putchar(negrilla);
}
/*------------------------------------------------
Cmd tamaño del caracter  impresora
00 tamaño normal
Altura
0x1	boble altura,0x02 = x3, 0x3 =x4, 0x4 =x5, 0x05=x6, 0x6=x7 0x7 =x8
Ancho
00 normal
0x10	=x2 -------------0x70=x8

como se debe aramr el cmd 00 normal, 11 boble de ancho y altura
------------------------------------------------*/

void Tama_letra(unsigned char tama)
{
	putchar(GS);
	putchar('!');
	putchar(tama);
}

/*------------------------------------------------
Cmd que corre  el papel N lineas 
------------------------------------------------*/

void Alimenta_nline(unsigned char nline)
{
	putchar(ESC);
	putchar('d');
	putchar(nline);
}
/*------------------------------------------------
Cmd que alimenta el  papel N lineas y lo posiciona para escribir 
------------------------------------------------*/
void Alimentar_paper(unsigned char nline)
{
	putchar(ESC);
	putchar('J');
	putchar(nline);
}
/*------------------------------------------------
Cmd de ESPACIOS ENTRE CARACTERES 
0= normal
1= separacion de espacios entre caracter
------------------------------------------------*/

void Espcs_ent_chr(unsigned char espacios)
{
	putchar(ESC);
	putchar(SP);
	putchar(espacios);
}
/*------------------------------------------------
Cmd que configura el tamaño del codigo de barras
el defaul es 162
------------------------------------------------*/
void height_code_bar(unsigned char height)
{
	putchar(GS);
	putchar('h');
	putchar(height);
}

/*------------------------------------------------
Cmd que imprime el codigo  de barras 
'0' no lleva 
'1' arriba
'2' abajo
'3' arriba y abajo
------------------------------------------------*/
void position_Hri(unsigned char position)
{
	putchar(GS);
	putchar('H');
	putchar(position);
}
/*------------------------------------------------
Cmd que busca la fuente de la letra
'0' font A (12 - 24)
'1' font B (9 -17)
------------------------------------------------*/
void Fount_char_hri()
{
	putchar(GS);
	putchar('f');
	putchar('0');
}
/*--------------------------------------------------

----------------------------------------------------*/
void Prin_cod_barra(unsigned char tipo,unsigned char *bar)
{
	unsigned char i;
		putchar(GS);
		putchar('k');
		putchar(tipo);
		for (i=0; bar[i] != '\0'; i++)
		{
 	  		putchar(bar[i]);
		}

	
		putchar(NUL);
}
/*------------------------------------------------------
funciones del codigo QR
--------------------------------------------------------*/
void Funcion_165()
{
	/*seleccion modelo QR (modelo seleccionado 2)(0x32=50)*/
	putchar(GS);
	putchar('(');
	putchar('k');
	putchar(EOT);
	putchar(NUL);
	putchar(0x31);
	putchar(0x41);
	putchar(0x32);
	putchar(NUL);
	
}

void Funcion_169()
{
/*seleccione el nivel  de correcciones de errores el codigo QR*/	
	putchar(GS);
	putchar('(');
	putchar('k');
	putchar(ETX);
	putchar(NUL);
	putchar(0x31);
	putchar(0x45);
	putchar(0x31);
	
}

void Funcion_167()
{
	/*se establece el tamaño del codigo QR*/
	putchar(GS);
	putchar('(');
	putchar('k');
	putchar(ETX);
	putchar(NUL);
	putchar(0x31);
	putchar(0x43);
	putchar(BEL);
	
}
void Funcion_180(unsigned char *datos)
{
	/*almacena el codigo qr*/
	unsigned char pl,i;
	putchar(GS);
	putchar('(');
	putchar('k');
	pl=strlen(datos);
	putchar(pl+3);
	putchar(NUL);
	putchar(0x31);
	putchar(0x50);
	putchar(0x30);
	for (i=0; i < pl; i++)
		{
 	  		putchar(datos[i]);
		}	
}
void Funcion_181()
{
	/*imprime el codigo QR*/
	putchar(GS);
	putchar('(');
	putchar('k');
	putchar(ETX);
	putchar(NUL);
	putchar(0x31);
	putchar(0x51);
	putchar(0x30);
}
/*------------------------------------------------
Cmd que corta   el papel  
------------------------------------------------*/
void FullCut(void)
{
	putchar(ESC);
	putchar('i');	
}
/*------------------------------------------------
Cmd que pregunta si hay papel
02= off line
04= estado del papel si hay o no
05= recoger papel

------------------------------------------------*/
void Status(unsigned char StatusConsulta)
{
 	
	putchar(DLE);
	putchar(EOT);
	putchar(StatusConsulta);
}
/*---------------------------------------------------------------------------------------------------------
se imprime el codigo QR
----------------------------------------------------------------------------------------------------------*/
void Print_cod_QR()
{
	unsigned char datos[80];
	unsigned char bar[15];
	
	Funcion_165();															/*selecciona el modelo del QR*/
	Funcion_169();															/*selecciona el nivel de correccion de errores*/
	
	Funcion_167();															/*establece el tamaño del QR*/

/*---------------------Se imprime el numero del ticket-------------------------------------------------	*/
	strcpy(datos, "Tiquete No: ");								/*datos del qr Tiquete No:*/
	serie_ascii_siceros_l(bar);									/*numero del tiquete*/
	strcat(datos,bar);													/*une los dos strim*/
	//strcat(datos,"\n");	
/*---------------------Se imprime la fecha de entrada del vehiculo-------------------------------------	*/	
	strcat(datos," Fecha: 20");
	Block_read_clock_ascii(bar);															/*Lee la fecha en ascci*/		
	strncat(datos,bar,2);																			/*Año*/
	strcat(datos,"-");
	strncat(datos,bar+2,2);																		/*mes*/
	strcat(datos,"-");																				
	strncat(datos,bar+4,2);																		/*dia*/
	strcat(datos," ");																				
	strncat(datos,bar+6,2);																		/*Hora*/
	strcat(datos,":");																				
	strncat(datos,bar+8,2);																		/*minutos*/
	//strcat(datos,":");																				
	//strncat(datos,bar+10,2);
	//strcat(datos,"\n");																				/*segundos*/
/*---------------------Se imprime el tipo de vehiculo -------------------------------------------------------	*/	
	if(Tipo_Vehiculo==0)
	{
		strcat(datos," Vehiculo: Carro");
	}
	else
	{
		strcat(datos," Vehiculo: Moto");
	}	
		
/*---------------------Se imprime el numero de placa del vehiculo-----------------------------------------------	*/
  strcat(datos,"Placa: ");
	strcat(datos,placa);
	strcat(datos,"\n");	
	Funcion_180(datos);													/*almacena el codigo QR*/	
	Funcion_181();															/*imprime el codigo Qr*/
}
/*-------------------------------------------------------------------------------------------------------------
Imprime el ticket en codigo QR
---------------------------------------------------------------------------------------------------------------*/
void Print_datos_cliente_QR()
{
	unsigned char buffer[50];
	/*------------------------------------------------imprime el nombre del parqueadero*/	
		Inicializa_Print();													/*cmd de inicio de impresora*/
		Posicion_tabulacion(0x35,0);								/*posiciona el carro*/
		Move_car();																	/*establece las unidades de movimiento H y V*/
		Justify('C');																/*texto justificado al centro*/
		Bold(01);																		/*pone el texto en negrilla*/
		Tama_letra(0x01);														/*tamaño del caracter*/						
		Espcs_ent_chr(01);													/*espacio entre caracteres*/
		Alimentar_paper(06);												/*alimenta el papel y se posiciona para imprimir*/
		LeerMemoria(EE_CLIENTE,buffer);	
		printf("%s",buffer);												/*imprimo el nombre de la  concepcion*/
/*------------------------------------------------*/	
		LeerMemoria(EE_NIT,buffer);	
		Bold(00);																		/*pone el texto sin negrilla*/
		Tama_letra(0x00);														/*tamaño del caracter*/
		Espcs_ent_chr(0);														/*espacio entre caracteres*/
		printf("NIT: %s",buffer);										/*imprimo numero de nit*/
/*------------------------------------------------*/			
		Justify('C');																/*texto justificado al centro*/
		LeerMemoria(EE_PARQUEADERO,buffer);	
		printf("%s",buffer);												/*imprimo el nombre del parqueadero*/
/*------------------------------------------------*/	
		Justify('C');																/*texto justificado al centro*/
		LeerMemoria(EE_ADDRES	,buffer);	
		printf("%s",buffer);												/*imprimo la direcccion y telefono del parqueadero*/
		Interlineado(10);	
/*------------------se crea el codigo QR-----------------------------------------------------------*/		
		Justify('C');																/*texto justificado al centro*/
		Print_cod_QR();		
/*--------------------------------------------------------------------------------------------------*/
		
		print_ticket();															/*Imprimo No de ticket, fecha, tipo de vehiculo, placa*/
		Print_msj_cod_barras();											/*imprime los msj informativos del ticket*/
		printf("\n\n\n\n\n\n");	
		FullCut();
}
/*------------------------------------------------
Cmd que imprime el ticket del codigo de barras
------------------------------------------------*/
void Print_datos_cliente()
{
	unsigned char buffer[50];
	
/*------------------------------------------------		codigo de barras al inicio*/
		Interlineado(2);	
		Alimenta_nline(1);
		Print_cod_barras();														/*Imprimo el codigo de barras con el numero del recibo */
/*------------------------------------------------imprime el nombre del parqueadero*/	
		Inicializa_Print();													/*cmd de inicio de impresora*/
		Posicion_tabulacion(0x35,0);								/*posiciona el carro*/
		Move_car();																	/*establece las unidades de movimiento H y V*/
		Justify('C');																/*texto justificado al centro*/
		Bold(01);																		/*pone el texto en negrilla*/
		Tama_letra(0x01);														/*tamaño del caracter*/						
		Espcs_ent_chr(01);													/*espacio entre caracteres*/
		Alimentar_paper(06);												/*alimenta el papel y se posiciona para imprimir*/
		LeerMemoria(EE_CLIENTE,buffer);	
		printf("%s",buffer);												/*imprimo el nombre de la  concepcion*/
/*------------------------------------------------*/	
		LeerMemoria(EE_NIT,buffer);	
		Bold(00);																		/*pone el texto sin negrilla*/
		Tama_letra(0x00);														/*tamaño del caracter*/
		Espcs_ent_chr(0);														/*espacio entre caracteres*/
		printf("NIT: %s",buffer);										/*imprimo numero de nit*/
/*------------------------------------------------*/			
		Justify('C');																/*texto justificado al centro*/
		LeerMemoria(EE_PARQUEADERO,buffer);	
		printf("%s",buffer);												/*imprimo el nombre del parqueadero*/
/*------------------------------------------------*/	
		Justify('C');																/*texto justificado al centro*/
		LeerMemoria(EE_ADDRES	,buffer);	
		printf("%s",buffer);												/*imprimo la direcccion y telefono del parqueadero*/
		Interlineado(10);		
		print_ticket();															/*Imprimo No de ticket, fecha, tipo de vehiculo, placa*/
		Print_msj_cod_barras();											/*imprime los msj informativos del ticket*/
		Print_cod_barras();														/*Imprimo el codigo de barras con el numero del recibo */
		printf("\n\n\n\n");	
		FullCut();
}
/*	------------------------------------------------------------------------
Con el numero de serie imprime el codigo de barras
----------------------------------------------------------------------------*/
void Print_cod_barras()
{
	unsigned char serie[10];
	Inicializa_Print();													/*cmd de inicio de impresora*/
	Justify('C');																/*texto justificado al centro*/
	height_code_bar(0x65);												/*altura del codigo de barras*/
	position_Hri('1');														/*posicion donde imprime el codigo de barras legible es abajo*/
	Fount_char_hri();															/*Tipo de fuente*/
	Tipo_fuente('1');		
	serie_ascii_siceros_l(serie);
	Prin_cod_barra(CODE39,serie);

}
/*-------------------------------------------------------------------------------
Imprime msj informativos del ticket
--------------------------------------------------------------------------------*/
void Print_msj_cod_barras()
{
	unsigned char msj_print[62];
	Tipo_fuente('1');															/*tipo de fuente 1 letra pequeña*/
	Interlineado(10);															/*Separacion entre renglon*/
	Justify('L');																	/*justificado a la izquierda*/
	Bold(0);																			/*sin negrita*/
	LeerMemoria(EE_MSJ1,msj_print);
	printf(" %s",msj_print);	
	
	LeerMemoria(EE_MSJ2,msj_print);
	printf(" %s",msj_print);	
	
	LeerMemoria(EE_MSJ3,msj_print);
	printf(" %s",msj_print);	
	
	LeerMemoria(EE_MSJ4,msj_print);
	printf(" %s",msj_print);	
	
	LeerMemoria(EE_MSJ5,msj_print);
	printf(" %s",msj_print);	
	
	LeerMemoria(EE_MSJ6,msj_print);
	printf(" %s",msj_print);	
	
	LeerMemoria(EE_MSJ7,msj_print);
	printf(" %s",msj_print);
	printf("pyp");	
}
/*-------------------------------------------------------------------------------
Imprime No de ticket, fecha, tipo de vehiculo y placa 
--------------------------------------------------------------------------------*/
void print_ticket()
{
	unsigned char bar[15];
	Interlineado(10);
/*---------------------Se imprime el numero del ticket-------------------------------------------------	*/
	
	Bold(01);																									/*pone el texto en negrilla*/
	Alimenta_nline(1);
	Justify('L');	
	printf("   Tiquete No: ");	
	Bold(0);
	serie_ascii_siceros_l(bar);																/*se imprime el numero de serie*/
	printf("%s\n",bar);	
/*---------------------Se imprime la fecha de entrada del vehiculo-------------------------------------	*/	
	Block_read_clock_ascii(bar);															/*Lee la fecha en ascci*/		
	Justify('L');
	Bold(1);
	printf("   Fecha:      ");																			
	Bold(0);
	printf("20");	
 	putchar(bar[0]);																					/*Se imprime el año*/
	putchar(bar[1]);
	putchar('-');																							/*se imprime el mes*/
	putchar(bar[2]);
	putchar(bar[3]);
	putchar('-');																							/*se imprime el dia*/
	putchar(bar[4]);
	putchar(bar[5]);
	putchar(' ');
/*---------------------Se imprime la hora de entrada del vehiculo-------------------------------------------	*/	

	putchar(bar[6]);																					/*se imprime la hora*/
	putchar(bar[7]);
	putchar(':');																							
	putchar(bar[8]);																					/*se imprime minutos*/
	putchar(bar[9]);

	putchar('\n');
	
/*---------------------Se imprime el tipo de vehiculo -------------------------------------------------------	*/	
	Bold(1);
	if(Tipo_Vehiculo==0)
	{
		printf("   Vehiculo:   Carro\n");
		
	}
	else
	{
		printf("   Vehiculo:   Moto\n");
	}
/*---------------------Se imprime el numero de placa del vehiculo-----------------------------------------------	*/
	Tama_letra(0x01);														/*tamaño del caracter*/						
	Espcs_ent_chr(01);													/*espacio entre caracteres*/
	Bold(1);
	
	printf("   Placa:     ");
	printf("%s\n",placa);	
	
	Tama_letra(0x0);														/*tamaño del caracter*/						
	Espcs_ent_chr(0);													/*espacio entre caracteres*/
	Bold(0);
}


/*-------------------------------------------------------------------------------------------------------------------------
procedimiento que mira el estado en que se encuentra la impresora
SEQ_INICIO=00 se detecta lapresencia vehicular 
SEQ_QUEST_PRINT=1 si fue  presionado el boton 
paso_una_vez=variable que habilita el audio una sola vez el de entrada
timer_paso_sensor2= cuenta un tiempo de espera a que los semsores sean liberados
---------------------------------------------------------------------------------------------------------------------------*/

void Genera_ticket(void)
{
	static unsigned char paso_una_vez=0;
	static unsigned char timer_paso_sensor2=0;
	switch (g_cEstadoImpresion)
	{
		case SEQ_INICIO:
		/*espera vehiculo en loop*/
			
		if (ValTimeOutCom==1)																																	/*tiempo de espera */
		{
			if (ValidaSensoresPaso()!=0)																												/*pregunto q alla presencia vehicular*/
			{
				Debug_txt_Tibbo((unsigned char *) "Vehiculo en el loop\r\n\r\n");									/* se encuentra un sensor activo*/
				PantallaLCD(BIENVENIDO);
				if(paso_una_vez==0)
				{
				
				send_portERR(AUDIO_IN);																														/*habilito el audio de entrada*/
				paso_una_vez=1;
				}														
				sel_Pulsa();																																			/*se pregunta por el pulsador*/
				
				if (DataIn==0)  																																	/*si es precionado */		  
				{
			  		
				 Debug_txt_Tibbo((unsigned char *) "Pulsador Activo\r\n");
																																													/*envia cmd a monitor de vehiculo de Entrada*/
				 g_cEstadoImpresion=SEQ_QUEST_PRINT;																							/*vamos al siguiente paso a con validar el cmd*/		
				}
				else
				{
					Debug_txt_Tibbo((unsigned char *) "Pulsador No Activo\r\n");
					ValTimeOutCom=TIME_PULSADOR;
					buffer_ready=0;																																		/* buffer del pto serie (0) inicia a esperar la trama*/
					g_cEstadoComSoft=ESPERA_RX;		
					g_cEstadoImpresion=SEQ_INICIO;																										/*volvemos a preguntar	*/	
							
				}
				
			
			}
			else
				{
				Debug_txt_Tibbo((unsigned char *) "Vehiculo no en el loop\r\n\r\n");							/* no hay vehiculo en el loop*/
				paso_una_vez=0;																								
				ValTimeOutCom=TIME_PULSADOR;
				buffer_ready=0;																																		/* buffer del pto serie (0) inicia a esperar la trama*/
				g_cEstadoComSoft=ESPERA_RX;		
				g_cEstadoImpresion=SEQ_INICIO;																										/*volvemos a preguntar	*/	
							
				}
		}
		break;
/*-----------------------------------------------------------------------------------------------------------------------------
SEQ_QUEST_PRINT=01 el boton fue presionado, envio cmd a monitor 	
-------------------------------------------------------------------------------------------------------------------------------*/
		case SEQ_QUEST_PRINT:
				
			Cmd_Monitor('E');																																/*trama monitor*/
			clear_buffer();																																	/*limpio el buffer del pto serie*/
			Status(04);																																			/*pregunta a  la impresora si tiene papel*/
			send_portERR(BIENVENIDO);																												/*puerto paralalelo*/
				
		
			g_cEstadoImpresion=SEQ_RTA_QUEST;
			ValTimeOutCom=TIME_RX	;																													/*tiempo de espera de respuesta de la impresora*/	
			buffer_ready=0;																																	/* buffer del pto serie (0) inicia a esperar la trama*/
			g_cEstadoComSoft=ESPERA_RX;		
			break;
/*-----------------------------------------------------------------------------------------------------------------------------
SEQ_RTA_QUEST=02 se pregunta si hay papel en la impresora
		el pto serie debe responder
		00= no hay datas
		01= si tiene papel la printer
		02= no, tiene papel     		
-------------------------------------------------------------------------------------------------------------------------------*/		
		case SEQ_RTA_QUEST:
		
		if ((ValTimeOutCom==1)||(buffer_ready!=0))
			{
				
				DebugBufferMF(Buffer_Rta_Lintech,1,1);		
				
				if (buffer_ready==1)
				{
					buffer_ready=0;
					Debug_txt_Tibbo((unsigned char *) "Impresora con papel \r\n");							/*la respuesta es desconocida*/
					ValTimeOutCom=TIME_PLACA;
					g_cEstadoImpresion=SEQ_PRINT	;		
				}
				else if (buffer_ready==2)
				{
						
				   Debug_txt_Tibbo((unsigned char *) "Impresora no tiene papel \r\n");				/*la respuesta es desconocida*/
					 ValTimeOutCom=TIME_CARD;
					 buffer_ready=0;																																		/* buffer del pto serie (0) inicia a esperar la trama*/
					 g_cEstadoComSoft=ESPERA_RX;		
					 g_cEstadoImpresion=SEQ_INICIO;
							
				}
				
				else if (buffer_ready==3)
				{
					 Debug_txt_Tibbo((unsigned char *) "Nivel bajo de papel \r\n");				/*la respuesta es desconocida*/
					 ValTimeOutCom=TIME_PLACA;
					 buffer_ready=0;																																		/* buffer del pto serie (0) inicia a esperar la trama*/
					 g_cEstadoComSoft=ESPERA_RX;		
					 g_cEstadoImpresion=SEQ_PRINT;
							
				}
				else 
				{
					 	Debug_txt_Tibbo((unsigned char *) "Impresora no responde \r\n");					/*la respuesta es desconocida*/
					 ValTimeOutCom=TIME_CARD;
					 buffer_ready=0;																																		/* buffer del pto serie (0) inicia a esperar la trama*/
					 g_cEstadoComSoft=ESPERA_RX;		
					 g_cEstadoImpresion=SEQ_INICIO;
						
				}
			}
				
			break;
/*-----------------------------------------------------------------------------------------------------------------------------
SEQ_PRINT	=03 se imprime la factura
y se envia una trama al pto paralelo para que el principal envie trama a access	
ademas incrementa el numero de consecutivo	
placa_ready=1 indica que tiene la placa =0 indica que no le ha llegado la placa			
-------------------------------------------------------------------------------------------------------------------------------*/		
			case SEQ_PRINT:
				if((ValTimeOutCom==1)|| (placa_ready==1))																					/*tiempo de espera */
			{
				Trama_pto_paralelo_printer('a');																									/*se envia trama al pto paralelo para comunicar access*/
				
		    send_portERR(AUDIO_ENTER);
				PantallaLCD(INGRESO);				
				Debug_txt_Tibbo((unsigned char *) "Imprimiendo \r\n");														/*la respuesta es desconocida*/
				if(QR_BARRAS!=0)																																	/* se pregunta si imprime codigo de barras o Qr*/
				{
				Print_datos_cliente_QR();																													/*imprime codigo QR*/	
				}
				else
				{
				Print_datos_cliente();																														/*imprime los datos del cliente codigo de barras*/
				}
				
				ValTimeOutCom=TIME_PULSADOR;
				buffer_ready=0;																																		/* buffer del pto serie (0) inicia a esperar la trama*/
				g_cEstadoComSoft=ESPERA_RX;		
				g_cEstadoImpresion=SEQ_RELEASE;
				lock=1;	
				Timer_wait=0;
				incremente_serie();																																/*se habre la barrera*/
			  clear_placa();
			}
			break;
/*-------------------------------------------------------------------------------------------------------------------------------
	espero el retorno de los sensores a esta normal 
y reinicio el ciclo			
---------------------------------------------------------------------------------------------------------------------------------*/		
			case SEQ_RELEASE:
			if (ValTimeOutCom==1)																																	/*tiempo de espera */
			{
				timer_paso_sensor2++;
			 if (timer_paso_sensor2 <= 18 )
			 {
			  sel_Pulsa();																																			/*se pregunta por el pulsador*/
				if (DataIn==1)  																																	/*que este desactivado*/
				{
					
					sel_Sensor2();																																	/*valido que la barrera este abajo*/
					if ((DataIn==0)||(timer_paso_sensor2 >= 18 ))  
					{
						timer_paso_sensor2=0;
						paso_una_vez=0;
						lock=0;																																						/*apago el rele de la salida */
						ValTimeOutCom=TIME_PULSADOR;
						buffer_ready=0;																																		/* buffer del pto serie (0) inicia a esperar la trama*/
						g_cEstadoComSoft=ESPERA_RX;
						g_cEstadoImpresion=SEQ_INICIO;
					}
					else 
					{
						PantallaLCD(INGRESO);	
						lock=0;
						ValTimeOutCom=TIME_PULSADOR;
						buffer_ready=0;																																		/* buffer del pto serie (0) inicia a esperar la trama*/
						g_cEstadoComSoft=ESPERA_RX;
						g_cEstadoImpresion=SEQ_RELEASE;
					}
				}
				else
				{
						
					ValTimeOutCom=TIME_PULSADOR;
					buffer_ready=0;																																		/* buffer del pto serie (0) inicia a esperar la trama*/
					g_cEstadoComSoft=ESPERA_RX;
					g_cEstadoImpresion=SEQ_RELEASE;
				}
			 }else
			 {
				timer_paso_sensor2=0;
				paso_una_vez=0;
				lock=0;																																						/*apago el rele de la salida */
				ValTimeOutCom=TIME_PULSADOR;
				buffer_ready=0;																																		/* buffer del pto serie (0) inicia a esperar la trama*/
				g_cEstadoComSoft=ESPERA_RX;
				g_cEstadoImpresion=SEQ_INICIO;
			 }
			}
		break;
				
					
			default:
			g_cEstadoImpresion=SEQ_INICIO;
		break;

	}
}
