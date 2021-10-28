#include <verprint.h>
#include <string.h>
#include <reg51.h>
/*funciones prototipo externas */

extern void Debug_txt_Tibbo(unsigned char * str);
extern void DebugBufferMF(unsigned char *str,unsigned char num_char,char io);
extern void Debug_chr_Tibbo(unsigned char Dat);
extern void PantallaLCD(unsigned char cod_msg);
extern void send_portERR(unsigned char cod_err);
extern unsigned char  ValidaSensoresPaso(void);
extern void clear_buffer();
extern void Trama_print_cod_barras(unsigned char *msj,unsigned char vehiculo);
extern void Cmd_LPR_Salida_print(unsigned char *msj,unsigned char vehiculo);
extern void Block_read_Clock_Hex(unsigned char *datos_clock);
extern unsigned char bcd_hex (unsigned char l_data);
extern void PantallaLCD(unsigned char cod_msg);
extern char lee_clk (unsigned char dir_clk);


/*variables externas*/
extern unsigned char g_cEstadoComSoft;
extern unsigned char g_cEstadoImpresion;
extern unsigned char ValTimeOutCom;
extern unsigned char buffer_ready;
extern idata unsigned char rbuf [];
extern	unsigned char g_cContByteRx;
extern  unsigned char  USE_LPR;
extern unsigned char Timer_wait;

#define DIRIJASE_CAJA						90
#define RHORA						0x85
#define RMIN						0x83
sbit lock = P1^7;						//Relevo 
/*----------------------------------------------------------------------------
tiempo de delay entre funciones
------------------------------------------------------------------------------*/

#define 	TIME_CARD					20		//50
#define 	TIME_RX						200		//
#define 	TIME_PLACA				55
#define 	TIME_PULSADOR			3

/*----------------------------------------------------------------------------
definicion de recepcion serial 
------------------------------------------------------------------------------*/

#define  ESPERA_RX 					0  					//espera el primer cmd de recepcion del verificado 
/*----------------------------------------------------------------------------
Definiciones de sequencias de lectura de ticket 
------------------------------------------------------------------------------*/

#define SEQ_INICIO						0X00	
#define SEQ_LEECODIGO					0X01
#define SEQ_SEND_SOFT					0X02
#define SEQ_SEND_SOFT_QR			0x03
#define SEQ_CMNCCN_PTO				0X04
#define SEQ_REINTENTO					0X05
/*---------------------------------------------------------------------------------
definiciones de la pantalla
-----------------------------------------------------------------------------------*/
#define INGRESO									0xFA
#define BIENVENIDO							0XFE
#define AUDIO_IN							  0XA0		// RELE 1
#define AUDIO_ENTER				      0XA1		// RELE 2
#define GRACIAS									0XFF


#define True										0x01
#define False										0x00
/*---------------------------------------------------------------------------------
funcion que debuelve la posicion del inicio del primer caracter de numerico de 0 a 9
-----------------------------------------------------------------------------------*/
unsigned char num_num(unsigned char * p)	
{
unsigned char contador=0;
	while ((*p < 0x30)|| (*p > 0x39))
	{
		
		p++;
		contador++;
	}
	return contador;
}		
/*---------------------------------------------------------------------------------
definiciones de la pantalla
-----------------------------------------------------------------------------------*/
unsigned char num_char(unsigned char * p,unsigned char chr)	
{
unsigned char contador=0;
	while (*p !=chr)
	{
		
		p++;
		contador++;
	}
	return contador;
}	
/*------------------------------------------------------------------------------------
Solo valida a�o, mes, y dia
------------------------------------------------------------------------------------*/
unsigned char check_fechaOut_2(unsigned char *buffer)
{
	unsigned long int fecha_inicio,fecha_fin;
	unsigned char datos_clk[6];
	char temp;
	
	
		Block_read_Clock_Hex(datos_clk);															/*leo el clock actual*/
 		fecha_inicio =	datos_clk[0] * 365 + datos_clk[1] * 30 + datos_clk[2] ;
		fecha_fin = *(buffer ) * 365 + *(buffer + 1) * 30  + *(buffer + 2);
		
			
		if (fecha_fin >= fecha_inicio	)						
		{
			temp = True;
		}
		else
		{
			temp = False;
		}
			

	
	return temp;
}
unsigned Bloque_Horario(unsigned char  * buffer)
{
	unsigned char Estado_Horario;	
	unsigned int HoraNow, Hora_Ticket;
	
	/*la hora del momento de entrada del vehiculo*/

	Debug_txt_Tibbo((unsigned char *) "HORA AHORA: ");
	Debug_chr_Tibbo(lee_clk(RHORA));
	Debug_chr_Tibbo(lee_clk(RMIN));
	Debug_txt_Tibbo((unsigned char *) "\r\n");
	HoraNow = (lee_clk(RHORA) * 60) + (lee_clk(RMIN) );
	
	/* desde la hora en que puede ingresar vehiculo */
	
	
	Debug_txt_Tibbo((unsigned char *) "HORA PROGRAMADA DESDE: ");
	Hora_Ticket = *(buffer+3)* 60 + *(buffer+4);
	if( HoraNow <= Hora_Ticket)
		{
			//send_portERR(PRMR_MENSUAL_FUERA_HORARIO);
			Debug_txt_Tibbo((unsigned char *) "EN HORARIO PROGRAMADO\r\n");
			Estado_Horario = True;
		}
		else
		{
			//send_portERR(PRMR_MENSUAL_FUERA_HORARIO);
			//PantallaLCD(MENSUAL_FUERA_HORARIO);
			Debug_txt_Tibbo((unsigned char *) "DESPUES DEL HORARIO PROGRAMADO\r\n");
			Estado_Horario = False;
		}
		return Estado_Horario;
}
/*-------------------------------------------------------------------------------------------------------------------------
procedimiento que lee el codigo de barra o el QR
SEQ_INICIO=00 se detecta la presencia vehicular 
SEQ_QUEST_PRINT=1 si fue  presionado el boton 
---------------------------------------------------------------------------------------------------------------------------*/

void Lee_ticket(void)
{
	static unsigned char paso_una_vez=0;
	static unsigned char Ticket[10];
	unsigned char fecha[11];
	static unsigned char fecha2[10];
	unsigned char temp,temp2,vehiculo;
	unsigned char *tipo_vehiculo;
	switch (g_cEstadoImpresion)
	{
		case SEQ_INICIO:
		/*detecta el vehiculo en el loop*/
			
		if (ValTimeOutCom==1)																																	/*tiempo de espera */
		{
			lock=0;
			
			if (ValidaSensoresPaso()!=0)																												/*pregunto q alla presencia vehicular*/
			{
				Debug_txt_Tibbo((unsigned char *) "\r\n\Vehiculo en el loop\r\n");									/* se encuentra un sensor activo*/
				PantallaLCD(BIENVENIDO);																													/*msj acerque el ticket*/
				if(paso_una_vez==0)
				{
				send_portERR(AUDIO_IN);																														/*habilito el audio de entrada*/
				paso_una_vez=1;
				}														
				ValTimeOutCom=TIME_RX;
				g_cEstadoImpresion=SEQ_LEECODIGO;																										/*volvemos a preguntar	*/	
							
				
			
			}
			else
				{
				Debug_txt_Tibbo((unsigned char *) "Vehiculo no en el loop\r\n\r\n");							/* no hay vehiculo en el loop*/
				paso_una_vez=0;																								
				ValTimeOutCom=TIME_PULSADOR;
				g_cEstadoImpresion=SEQ_INICIO;																										/*volvemos a preguntar	*/	
							
				}
		}
		break;
/*-------------------------------------------------------------------------------------------------------------------------------------------
SEQ_LEECODIGO	
lee el dato en el pto serial del codigo qr
----------------------------------------------------------------------------------------------------------------------------------------------*/		
		case SEQ_LEECODIGO:
			if ((ValTimeOutCom == 1)||(buffer_ready != 0))
			{
				if (buffer_ready == 1)
				{
					/*trama con codigo QR*/
					Debug_txt_Tibbo((unsigned char *) "trama del lector datalogic cod QR");							/*la respuesta es desconocida*/
					Debug_txt_Tibbo(rbuf);																															/*imprimo la trama recibida*/
					Debug_txt_Tibbo((unsigned char *) "\r\n");
					ValTimeOutCom=TIME_CARD;																						
					g_cEstadoImpresion=SEQ_SEND_SOFT_QR	;		
																																															/* buffer del pto serie (0) inicia a esperar la trama*/
				 
				}
				else if (buffer_ready == 2)
				{
					/*codigo de barras*/
					strcpy(Ticket,rbuf);																																/*salvo el buffer*/
					temp=strlen(rbuf);																																	/*longitud del buffer*/
					Ticket[temp-1]=0;
					if (temp>10)
					{
					clear_buffer();
					PantallaLCD(BIENVENIDO);
					ValTimeOutCom=TIME_RX;
					g_cEstadoImpresion=SEQ_LEECODIGO;	
					break;
					}
					/*COD DE BARRAS*/	
					Debug_txt_Tibbo((unsigned char *) "trama del lector datalogic cod barras");						/*la respuesta es desconocida*/
					Debug_txt_Tibbo(Ticket);																																/*imprimo la trama recibida*/
					Debug_txt_Tibbo((unsigned char *) "\r\n");
																										
				
																																									/* buffer del pto serie (0) inicia a esperar la trama*/
				  
					g_cEstadoImpresion=SEQ_SEND_SOFT	;		
				}
				else
				{
					/*respuesta incorrecta limpia buffer lee otra vez*/
					clear_buffer();
					paso_una_vez=0;
					buffer_ready=0;		
					Debug_txt_Tibbo((unsigned char *) "trama del lector datalogic no esta completa\r\n");							/*la respuesta es desconocida*/
					ValTimeOutCom=TIME_PULSADOR;
					g_cEstadoImpresion=SEQ_INICIO;	
					}
								
			
				
			} 
		break;
		case SEQ_SEND_SOFT:
			
				/*codigo de barras*/
				Debug_txt_Tibbo((unsigned char *) "cod barras");	
				buffer_ready=0;	
				vehiculo='C';
				Trama_print_cod_barras(Ticket,vehiculo);
		
			if(USE_LPR)
				{
				Cmd_LPR_Salida_print(Ticket,vehiculo);																								/*envio datos a Monitor*/
				}
				ValTimeOutCom=TIME_PLACA;
				g_cEstadoImpresion=SEQ_CMNCCN_PTO	;		
			
			
			break;
			
		case  SEQ_SEND_SOFT_QR:
			/*es un codigo QR*/
			//	ES = 0;															/*inactivo pto serie y analizo el dato*/
			if (ValTimeOutCom==1)
			{
		
				Debug_txt_Tibbo((unsigned char *) "cod QR r\n");
				/*codigo qr*/
				buffer_ready=0;											/*limpio el testigo de recepcion de datos serie*/
				
				temp=num_num(rbuf);									/*funcion que pregunta donde empieza el primer numero del ticket*/
			  temp2=num_char(rbuf+temp,'>');			/*busca un caracter en la trama*/
				if ((tipo_vehiculo=strstr(rbuf,"Carro"))!= 0)	/*pregunto el tipo de vehiculo grabado en el codigo QR*/
					{
					vehiculo='C';
					}
				else
					{
					vehiculo='M';
					}
				Debug_txt_Tibbo((unsigned char *) "tipo de vehiculo: ");						/*msj tipo de vehiculo */
				Debug_chr_Tibbo(vehiculo);																					/*caracter del tipo de vehiculo*/
				Debug_txt_Tibbo((unsigned char *) "\r\n");													/*final de linea*/
				
				Debug_txt_Tibbo((unsigned char *) "longitud de la trama: \r\n");		/*msj longitud de la trama */
				Debug_chr_Tibbo(temp);																							/*numero de inicio del ticket*/
				Debug_txt_Tibbo((unsigned char *) "\r\n");													/*final de linea*/													
				Debug_chr_Tibbo(temp2);																							/*numero de caracteres del ticket*/
				Debug_txt_Tibbo((unsigned char *) "\r\n");													/*final de linea*/
		   
				Ticket[0]=0;
				if(temp== 0x0c)																											/*la trama debe iniciar en 0x0c*/
				{	
				strncpy(Ticket,rbuf+temp,temp2);																		/*copio el numero de ticket*/
				Ticket[temp2]=0;																										/*finalizo la trama con (0)*/
								
				Debug_txt_Tibbo(Ticket);																						/*imprimo el numero de ticket*/
				Debug_txt_Tibbo((unsigned char *) "\r\n");													/*final de linea*/
				/*----------------------------------------------------*/
				/*buscamos la fecha de salida  en el ticket*/
					
					temp=num_char(rbuf+temp,':')+temp;
					Debug_chr_Tibbo(temp);	
					temp2=num_char(rbuf+temp,'>');
					Debug_chr_Tibbo(temp2);																							/*numero de caracteres del ticket*/
					Debug_txt_Tibbo((unsigned char *) "\r\n");	
					strncpy(fecha,rbuf+temp+2,temp2);
					fecha[temp2-2]=0;
					/*a�o*/
					fecha2[0]=bcd_hex((fecha[2]-0x30)<<4 | fecha[3]-0x30);
					/*mes*/
					fecha2[1]=bcd_hex((fecha[5]-0x30)<<4 | fecha[6]-0x30);
					/*dia*/
					fecha2[2]=bcd_hex((fecha[8]-0x30)<<4 | fecha[9]-0x30);
					/*horas*/
					fecha2[3]=bcd_hex((fecha[0x0b]-0x30)<<4 | fecha[0xc]-0x30);
					/*minutos*/
					fecha2[4]=bcd_hex((fecha[0x0e]-0x30)<<4 | fecha[0xf]-0x30);
					/*--------------------------------------------------*/
				Debug_txt_Tibbo((unsigned char *) "Fecha de salida: ");						/*msj tipo de vehiculo */
				Debug_txt_Tibbo((unsigned char *)fecha);																					/*caracter del tipo de vehiculo*/
				Debug_txt_Tibbo((unsigned char *) "\n");													/*final de linea*/					
					
				if (check_fechaOut_2(fecha2)!= True)
				{
					/*dirijase a caja*/
					PantallaLCD(DIRIJASE_CAJA);
				}
				if(Bloque_Horario(fecha2) != True)
				{
					/*dirijase a caja*/
					PantallaLCD(DIRIJASE_CAJA);
				}
				Trama_print_cod_barras(Ticket,vehiculo);														/*envio la trama al pto paralelo y es enviada al principal el cual comunica con acces*/
				
				
				if(USE_LPR)
				{
				Cmd_LPR_Salida_print(Ticket,vehiculo);																/*envio datos a Monitor*/
				}
				ValTimeOutCom=TIME_PLACA;
				g_cEstadoImpresion=SEQ_CMNCCN_PTO	;	
				break;
			  }
				else
				{
					ES = 1;
					clear_buffer();
				//	PantallaLCD(BIENVENIDO);
					//send_portERR(TARJETA_INVALIDA);	 
					ValTimeOutCom=TIME_RX;
					g_cEstadoImpresion=SEQ_LEECODIGO;		
				
				}
			}
					break;
		case SEQ_CMNCCN_PTO:
			lock=1;
			if (ValTimeOutCom==1)
			{
				lock=0;
				ES = 1;																																							/*habilito pto*/
				Debug_txt_Tibbo((unsigned char *) "fin de lectura de trama\r\n");										/*la respuesta es desconocida*/
				//PantallaLCD(GRACIAS);
				ValTimeOutCom=TIME_PULSADOR;
				clear_buffer();
				paso_una_vez=0;			 
				g_cEstadoImpresion=SEQ_INICIO;
			}
			break;
		default:
		g_cEstadoImpresion=SEQ_INICIO;
		break;
	
		case SEQ_REINTENTO:
			if (ValTimeOutCom==1)
			{
				Debug_txt_Tibbo((unsigned char *) "REENVIA TRAMA PTO PARALELO\r\n ");						/*msj tipo de vehiculo */
				
				Trama_print_cod_barras(Ticket,vehiculo);	
				ValTimeOutCom=TIME_PLACA;
				g_cEstadoImpresion=SEQ_CMNCCN_PTO	;	
			}
				break;		
	}
}